package com.nuitar.websocket;


import com.fasterxml.jackson.databind.ObjectMapper;
import com.nuitar.pojo.Player;
import com.nuitar.utils.Result;
import jakarta.websocket.*;
import jakarta.websocket.Session;
import jakarta.websocket.server.PathParam;
import jakarta.websocket.server.ServerEndpoint;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.actuate.autoconfigure.metrics.MetricsProperties;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;


import java.io.IOException;
import java.lang.reflect.Array;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

/**
 * WebSocket服务
 */
@ServerEndpoint("/ws/{sid}")
@Component
public class WebSocketServer {

    private ObjectMapper objectMapper = new ObjectMapper();

    //存放会话对象
    private static Map<String, Session> sessionMap = new ConcurrentHashMap<>();
    private static Map<String, Player> playersMap = new ConcurrentHashMap<>();


    @Autowired
    private RedisTemplate redisTemplate;

    /**
     * 连接建立成功调用的方法
     */
    @OnOpen
    public void onOpen(Session session, @PathParam("sid") String sid) {
        sessionMap.put(sid, session);

        System.out.println("客户端：" + sid + "建立连接" + ",当前连接数量:" + sessionMap.size());
    }

    /**
     * 收到客户端消息后调用的方法
     *
     * @param message 客户端发送过来的消息
     */
    @OnMessage
    public void synchPlayers(String message, @PathParam("sid") String sid) throws IOException {
//        System.out.println("收到来自客户端：" + sid + "的信息:" + message);
        Player player = objectMapper.readValue(message, Player.class);
        playersMap.put(sid,player);

        Collection<Player> values = playersMap.values();
        ArrayList<Player> valuesList = new ArrayList<Player>(values);
        sendAllPlayers(sessionMap.get(sid),valuesList);
    }

    /**
     * 连接关闭调用的方法
     *
     * @param sid
     */
    @OnClose
    public void onClose(@PathParam("sid") String sid) {
        System.out.println("连接断开:" + sid);
        sessionMap.remove(sid);
        playersMap.remove(sid);
    }


    public void sendAllPlayers(Session session,ArrayList<Player>players) {
        try {
            session.getBasicRemote().sendText(objectMapper.writeValueAsString(Result.success(players)));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

}
