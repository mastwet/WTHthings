use super::*;
use tempfile::tempdir;
use tokio::time::{sleep, Duration};

#[tokio::test]
async fn test_client_server_communication() -> Result<()> {
    // 创建临时目录用于测试
    let temp_dir = tempdir()?;
    let temp_path = temp_dir.path().to_str().unwrap().to_string();
    
    // 启动服务器
    let server = Server::new("127.0.0.1:8080").await?;
    
    // 创建客户端配置
    let client_config = ClientConfig {
        server_addr: "127.0.0.1:8080".to_string(),
        server_name: "localhost".to_string(),
        data_dir: Some(temp_path),
        timeout: Duration::from_secs(5),
        keep_alive: Duration::from_secs(30),
    };
    
    // 创建客户端
    let mut client = Client::new(client_config).await?;
    
    // 连接到服务器
    client.connect().await?;
    
    // 注册新用户
    let username = "test_user";
    let user_id = client.register(username).await?;
    assert!(!user_id.is_empty(), "User ID should not be empty");
    
    // 断开连接
    client.disconnect().await?;
    
    // 重新连接并登录
    client.connect().await?;
    let login_success = client.login(&user_id).await?;
    assert!(login_success, "Login should succeed");
    
    // 发送消息
    let test_message = "Hello, server!";
    client.send_message("server", test_message).await?;
    
    // 接收消息
    sleep(Duration::from_secs(1)).await; // 等待消息处理
    if let Some(received_message) = client.receive_message().await? {
        assert_eq!(received_message, "Hello from server!", "Received message should match");
    } else {
        panic!("No message received");
    }
    
    // 关闭服务器
    server.shutdown().await?;
    
    Ok(())
}

#[tokio::test]
async fn test_multiple_clients_communication() -> Result<()> {
    // 创建临时目录用于测试
    let temp_dir1 = tempdir()?;
    let temp_dir2 = tempdir()?;
    let temp_path1 = temp_dir1.path().to_str().unwrap().to_string();
    let temp_path2 = temp_dir2.path().to_str().unwrap().to_string();
    
    // 启动服务器
    let server = Server::new("127.0.0.1:8081").await?;
    
    // 创建客户端1
    let client1_config = ClientConfig {
        server_addr: "127.0.0.1:8081".to_string(),
        server_name: "localhost".to_string(),
        data_dir: Some(temp_path1),
        timeout: Duration::from_secs(5),
        keep_alive: Duration::from_secs(30),
    };
    
    let mut client1 = Client::new(client1_config).await?;
    
    // 创建客户端2
    let client2_config = ClientConfig {
        server_addr: "127.0.0.1:8081".to_string(),
        server_name: "localhost".to_string(),
        data_dir: Some(temp_path2),
        timeout: Duration::from_secs(5),
        keep_alive: Duration::from_secs(30),
    };
    
    let mut client2 = Client::new(client2_config).await?;
    
    // 客户端1连接并注册
    client1.connect().await?;
    let user_id1 = client1.register("user1").await?;
    
    // 客户端2连接并注册
    client2.connect().await?;
    let user_id2 = client2.register("user2").await?;
    
    // 客户端1向客户端2发送消息
    let test_message = "Hello from client1!";
    client1.send_message(&user_id2, test_message).await?;
    
    // 客户端2接收消息
    sleep(Duration::from_secs(1)).await; // 等待消息处理
    if let Some(received_message) = client2.receive_message().await? {
        assert_eq!(received_message, test_message, "Received message should match");
    } else {
        panic!("No message received by client2");
    }
    
    // 客户端2回复消息
    let reply_message = "Hello from client2!";
    client2.send_message(&user_id1, reply_message).await?;
    
    // 客户端1接收回复
    sleep(Duration::from_secs(1)).await; // 等待消息处理
    if let Some(received_reply) = client1.receive_message().await? {
        assert_eq!(received_reply, reply_message, "Received reply should match");
    } else {
        panic!("No reply received by client1");
    }
    
    // 关闭服务器
    server.shutdown().await?;
    
    Ok(())
}    