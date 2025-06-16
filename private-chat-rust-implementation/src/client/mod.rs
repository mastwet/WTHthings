use std::{sync::Arc, time::Duration};
use anyhow::{Result, Context};
use tokio::{sync::mpsc, time::sleep};
use log::{info, error};
use super::{
    auth::AuthService,
    crypto::key_manager::KeyManager,
    protocol::packet::PacketHandler,
    transport::TcpTransport,
    proto::messages,
};

// 客户端配置
pub struct ClientConfig {
    pub server_addr: String,
    pub server_name: String,
    pub data_dir: Option<String>,
    pub timeout: Duration,
    pub keep_alive: Duration,
}

// 客户端
pub struct Client {
    auth_service: AuthService,
    packet_handler: PacketHandler,
    transport: TcpTransport,
    message_sender: mpsc::Sender<Vec<u8>>,
    message_receiver: Option<mpsc::Receiver<Vec<u8>>>,
}

impl Client {
    // 创建新客户端
    pub async fn new(config: ClientConfig) -> Result<Self> {
        // 初始化密钥管理器
        let data_dir = config.data_dir.map(|s| std::path::PathBuf::from(s));
        let key_manager = KeyManager::new(data_dir)?;
        
        // 初始化传输层
        let transport_config = super::transport::TcpTransportConfig {
            server_addr: config.server_addr,
            server_name: config.server_name,
            timeout: config.timeout,
            keep_alive: config.keep_alive,
        };
        
        let transport = TcpTransport::new(transport_config);
        
        // 初始化认证服务
        let auth_service = AuthService::new(key_manager, transport.clone());
        
        // 初始化包处理器
        let packet_handler = PacketHandler::new();
        
        // 创建消息通道
        let (message_sender, message_receiver) = mpsc::channel(100);
        
        Ok(Self {
            auth_service,
            packet_handler,
            transport,
            message_sender,
            message_receiver: Some(message_receiver),
        })
    }
    
    // 连接到服务器
    pub async fn connect(&mut self) -> Result<()> {
        info!("Connecting to server...");
        self.transport.connect().await?;
        info!("Connected to server successfully");
        Ok(())
    }
    
    // 注册新用户
    pub async fn register(&mut self, username: &str) -> Result<String> {
        info!("Registering new user: {}", username);
        let user_id = self.auth_service.register(username).await?;
        info!("User registered successfully: {}", user_id);
        Ok(user_id)
    }
    
    // 用户登录
    pub async fn login(&mut self, user_id: &str) -> Result<bool> {
        info!("Logging in as user: {}", user_id);
        let is_success = self.auth_service.login(user_id).await?;
        
        if is_success {
            info!("Login successful");
            // 启动消息接收处理
            self.start_message_receiver().await?;
        } else {
            error!("Login failed");
        }
        
        Ok(is_success)
    }
    
    // 发送消息
    pub async fn send_message(&self, receiver_id: &str, content: &str) -> Result<()> {
        // 构建消息
        let message = messages::Message {
            sender_id: self.auth_service.user_id.clone().unwrap_or_default(),
            receiver_id: receiver_id.to_string(),
            content: content.to_string(),
            timestamp: chrono::Utc::now().timestamp_millis() as u64,
            message_type: messages::MessageType::Text as i32,
        };
        
        // 序列化消息
        let mut message_bytes = Vec::new();
        message.encode_length_delimited(&mut message_bytes)?;
        
        // 创建加密消息
        // 注意：在实际应用中，这里需要使用会话密钥进行加密
        let encrypted_message = self.packet_handler.create_encrypted_message(
            message.sender_id.as_bytes(),
            message.receiver_id.as_bytes(),
            messages::MessageType::Text,
            &message_bytes,
            &[0u8; 32], // 示例会话密钥
        )?;
        
        // 序列化加密消息
        let encrypted_bytes = self.packet_handler.serialize_message(&encrypted_message)?;
        
        // 发送消息
        self.transport.send(&encrypted_bytes)?;
        
        info!("Message sent to {}", receiver_id);
        Ok(())
    }
    
    // 接收消息
    pub async fn receive_message(&mut self) -> Result<Option<String>> {
        if let Some(receiver) = self.message_receiver.as_mut() {
            if let Some(message_bytes) = receiver.recv().await {
                // 解密消息
                // 注意：在实际应用中，这里需要使用会话密钥进行解密
                let decrypted_message = self.packet_handler.verify_and_decrypt_message(
                    &self.packet_handler.deserialize_message(&message_bytes)?,
                    &[0u8; 32], // 示例会话密钥
                )?;
                
                // 解析消息
                let message = messages::Message::decode_length_delimited(&*decrypted_message)?;
                
                info!("Received message from {}", message.sender_id);
                Ok(Some(message.content))
            } else {
                Ok(None)
            }
        } else {
            Err(anyhow::anyhow!("Message receiver not initialized"))
        }
    }
    
    // 断开连接
    pub async fn disconnect(&self) -> Result<()> {
        info!("Disconnecting from server...");
        self.transport.disconnect().await?;
        info!("Disconnected from server successfully");
        Ok(())
    }
    
    // 启动消息接收处理
    async fn start_message_receiver(&self) -> Result<()> {
        let message_sender = self.message_sender.clone();
        let transport = self.transport.clone();
        
        tokio::spawn(async move {
            loop {
                // 模拟接收消息
                // 在实际应用中，这里应该从传输层接收数据
                sleep(Duration::from_secs(1)).await;
                
                // 构建一个示例消息
                let example_message = messages::Message {
                    sender_id: "server".to_string(),
                    receiver_id: "client".to_string(),
                    content: "Hello from server!".to_string(),
                    timestamp: chrono::Utc::now().timestamp_millis() as u64,
                    message_type: messages::MessageType::Text as i32,
                };
                
                // 序列化消息
                let mut message_bytes = Vec::new();
                example_message.encode_length_delimited(&mut message_bytes)?;
                
                // 发送到消息通道
                if let Err(e) = message_sender.send(message_bytes).await {
                    error!("Failed to send message to channel: {}", e);
                    break;
                }
            }
        });
        
        Ok(())
    }
}    