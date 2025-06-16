use std::{
    sync::{Arc, Mutex},
    net::{TcpListener, TcpStream},
    collections::HashMap,
};
use anyhow::{Result, Context};
use tokio::{sync::mpsc, net::TcpListener as AsyncTcpListener};
use log::{info, error, warn};
use super::{
    crypto::key_manager::KeyManager,
    protocol::packet::PacketHandler,
    proto::messages,
};

// 客户端连接
struct ClientConnection {
    stream: TcpStream,
    user_id: Option<String>,
    packet_handler: PacketHandler,
}

// 服务器
pub struct Server {
    listener: AsyncTcpListener,
    key_manager: KeyManager,
    clients: Mutex<HashMap<String, ClientConnection>>,
    shutdown_rx: Mutex<Option<mpsc::Receiver<()>>>,
}

impl Server {
    // 创建新服务器
    pub async fn new(address: &str) -> Result<Self> {
        // 初始化密钥管理器
        let key_manager = KeyManager::new(None)?;
        
        // 绑定监听地址
        let listener = AsyncTcpListener::bind(address).await?;
        info!("Server listening on {}", address);
        
        // 创建关闭信号接收器
        let (shutdown_tx, shutdown_rx) = mpsc::channel(1);
        
        let server = Self {
            listener,
            key_manager,
            clients: Mutex::new(HashMap::new()),
            shutdown_rx: Mutex::new(Some(shutdown_rx)),
        };
        
        // 启动服务器
        tokio::spawn(server.clone().run(shutdown_tx));
        
        Ok(server)
    }
    
    // 运行服务器
    async fn run(mut self, shutdown_tx: mpsc::Sender<()>) {
        // 处理关闭信号的任务
        let shutdown_rx = self.shutdown_rx.lock().unwrap().take().unwrap();
        let shutdown_handle = tokio::spawn(async move {
            let _ = shutdown_rx.recv().await;
            info!("Server received shutdown signal");
        });
        
        // 接受客户端连接的任务
        let accept_handle = tokio::spawn(async move {
            loop {
                // 接受新连接
                let (stream, addr) = match self.listener.accept().await {
                    Ok(result) => result,
                    Err(e) => {
                        error!("Failed to accept connection: {}", e);
                        continue;
                    }
                };
                
                info!("New client connected: {}", addr);
                
                // 处理客户端连接
                let key_manager = self.key_manager.clone();
                let mut clients = self.clients.lock().unwrap();
                
                // 创建客户端连接
                let client_id = format!("client_{}", clients.len());
                let client_connection = ClientConnection {
                    stream: stream.into_std().unwrap(),
                    user_id: None,
                    packet_handler: PacketHandler::new(),
                };
                
                clients.insert(client_id.clone(), client_connection);
                
                // 为客户端启动处理任务
                let mut clients_clone = self.clients.lock().unwrap().clone();
                tokio::spawn(async move {
                    if let Err(e) = Self::handle_client(client_id, &mut clients_clone).await {
                        error!("Error handling client: {}", e);
                    }
                });
            }
        });
        
        // 等待任一任务完成
        tokio::select! {
            _ = shutdown_handle => {
                // 关闭服务器
                let _ = self.listener.shutdown();
                info!("Server shutdown complete");
            }
            _ = accept_handle => {
                // 接受任务完成，可能是错误导致的
                error!("Server accept task completed unexpectedly");
                let _ = shutdown_tx.send(()).await;
            }
        }
    }
    
    // 处理客户端连接
    async fn handle_client(client_id: String, clients: &mut HashMap<String, ClientConnection>) -> Result<()> {
        let mut client = clients.get_mut(&client_id)
            .context("Client not found")?;
        
        // 读取客户端消息
        let mut buffer = [0u8; 4096];
        loop {
            match client.stream.read(&mut buffer) {
                Ok(0) => {
                    // 客户端关闭连接
                    info!("Client disconnected: {}", client_id);
                    break;
                }
                Ok(n) => {
                    // 处理接收到的数据
                    let data = &buffer[..n];
                    Self::process_client_data(client_id.clone(), data, clients).await?;
                }
                Err(e) => {
                    error!("Error reading from client: {}", e);
                    break;
                }
            }
        }
        
        // 移除客户端
        clients.remove(&client_id);
        Ok(())
    }
    
    // 处理客户端数据
    async fn process_client_data(
        client_id: String,
        data: &[u8],
        clients: &mut HashMap<String, ClientConnection>,
    ) -> Result<()> {
        let client = clients.get_mut(&client_id)
            .context("Client not found")?;
        
        // 解析消息类型
        let message_type = match data.first() {
            Some(t) => *t,
            None => return Err(anyhow::anyhow!("Empty message")),
        };
        
        match message_type {
            // 处理注册请求
            messages::MessageType::RegisterRequest as u8 => {
                Self::handle_register_request(client_id.clone(), &data[1..], clients).await?;
            }
            
            // 处理登录请求
            messages::MessageType::LoginRequest as u8 => {
                Self::handle_login_request(client_id.clone(), &data[1..], clients).await?;
            }
            
            // 处理消息
            messages::MessageType::EncryptedMessage as u8 => {
                Self::handle_encrypted_message(client_id.clone(), &data[1..], clients).await?;
            }
            
            _ => {
                warn!("Unknown message type: {}", message_type);
            }
        }
        
        Ok(())
    }
    
    // 处理注册请求
    async fn handle_register_request(
        client_id: String,
        data: &[u8],
        clients: &mut HashMap<String, ClientConnection>,
    ) -> Result<()> {
        // 解析注册请求
        let request = messages::RegisterRequest::decode_length_delimited(data)?;
        info!("Register request received for user: {}", request.username);
        
        // 这里应该验证用户信息并存储用户公钥
        // 简化示例，直接返回成功
        
        // 构建注册响应
        let response = messages::RegisterResponse {
            success: true,
            user_id: request.user_id.clone(),
            message: "Registration successful".to_string(),
        };
        
        // 发送响应
        let mut response_bytes = Vec::new();
        response_bytes.put_u8(messages::MessageType::RegisterResponse as u8);
        response.encode_length_delimited(&mut response_bytes)?;
        
        let client = clients.get_mut(&client_id)
            .context("Client not found")?;
        
        client.stream.write_all(&response_bytes)?;
        
        // 更新客户端信息
        client.user_id = Some(request.user_id);
        
        info!("User registered successfully: {}", request.user_id);
        Ok(())
    }
    
    // 处理登录请求
    async fn handle_login_request(
        client_id: String,
        data: &[u8],
        clients: &mut HashMap<String, ClientConnection>,
    ) -> Result<()> {
        // 解析登录请求
        let request = messages::LoginRequest::decode_length_delimited(data)?;
        info!("Login request received for user: {}", request.user_id);
        
        // 生成挑战码
        let mut challenge = [0u8; 32];
        rand::thread_rng().fill(&mut challenge[..]);
        
        // 构建挑战响应
        let response = messages::LoginChallenge {
            user_id: request.user_id.clone(),
            challenge: challenge.to_vec(),
        };
        
        // 发送响应
        let mut response_bytes = Vec::new();
        response_bytes.put_u8(messages::MessageType::LoginChallenge as u8);
        response.encode_length_delimited(&mut response_bytes)?;
        
        let client = clients.get_mut(&client_id)
            .context("Client not found")?;
        
        client.stream.write_all(&response_bytes)?;
        
        info!("Challenge sent to user: {}", request.user_id);
        Ok(())
    }
    
    // 处理加密消息
    async fn handle_encrypted_message(
        client_id: String,
        data: &[u8],
        clients: &mut HashMap<String, ClientConnection>,
    ) -> Result<()> {
        let client = clients.get_mut(&client_id)
            .context("Client not found")?;
        
        // 解析加密消息
        let encrypted_message = client.packet_handler.deserialize_message(data)?;
        
        // 验证并解密消息
        // 注意：在实际应用中，这里需要使用会话密钥进行解密
        let decrypted_message = client.packet_handler.verify_and_decrypt_message(
            &encrypted_message,
            &[0u8; 32], // 示例会话密钥
        )?;
        
        // 解析消息
        let message = messages::Message::decode_length_delimited(&*decrypted_message)?;
        
        info!("Received message from {} to {}", message.sender_id, message.receiver_id);
        
        // 转发消息给接收者
        if let Some(receiver_id) = find_client_by_user_id(&message.receiver_id, clients) {
            let receiver = clients.get_mut(&receiver_id)
                .context("Receiver not found")?;
            
            // 转发加密消息
            receiver.stream.write_all(data)?;
            info!("Message forwarded to {}", message.receiver_id);
        } else {
            warn!("Receiver not found: {}", message.receiver_id);
        }
        
        Ok(())
    }
    
    // 关闭服务器
    pub async fn shutdown(self) -> Result<()> {
        if let Some(mut rx) = self.shutdown_rx.lock().unwrap().take() {
            // 发送关闭信号
            let _ = rx.try_recv();
        }
        
        Ok(())
    }
}

// 查找客户端ID
fn find_client_by_user_id(
    user_id: &str,
    clients: &HashMap<String, ClientConnection>,
) -> Option<String> {
    clients.iter()
        .find_map(|(client_id, conn)| {
            if conn.user_id.as_ref().map(|id| id == user_id).unwrap_or(false) {
                Some(client_id.clone())
            } else {
                None
            }
        })
}

// 克隆Server
impl Clone for Server {
    fn clone(&self) -> Self {
        Self {
            listener: self.listener.clone(),
            key_manager: self.key_manager.clone(),
            clients: self.clients.clone(),
            shutdown_rx: self.shutdown_rx.clone(),
        }
    }
}    