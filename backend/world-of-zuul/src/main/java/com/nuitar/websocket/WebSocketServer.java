package com.nuitar.websocket;


import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.nuitar.pojo.Player;
import com.nuitar.pojo.Room;
import com.nuitar.pojo.VO.SendDataVO;
import com.nuitar.pojo.VO.SyncDataVO;
import com.nuitar.utils.Result;
import jakarta.websocket.*;
import jakarta.websocket.Session;
import jakarta.websocket.server.PathParam;
import jakarta.websocket.server.ServerEndpoint;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;


import java.io.IOException;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/**
 * WebSocket服务
 */
@ServerEndpoint("/ws/{roomId}/{uuid}")
@Component
public class WebSocketServer {

    private ObjectMapper objectMapper = new ObjectMapper();

    // 每个roomId对应每组房间
    private static Map<String, List<List<Room>>> roomIdToRooms = new ConcurrentHashMap<>();

    //每个roomId对应联机的玩家
    private static Map<String, Map<String, Player>> roomIdToUuidToPlayer = new ConcurrentHashMap<>();
    @Autowired
    private RedisTemplate redisTemplate;

    /**
     * 连接建立成功调用的方法
     */
    @OnOpen
    public void onOpen(Session session, @PathParam("roomId") String roomId, @PathParam("uuid") String uuid) throws IOException {
        if (!roomIdToRooms.containsKey(roomId)) {
            roomIdToRooms.put(roomId, Room.generateRooms(5, 5));
            roomIdToUuidToPlayer.put(roomId, new ConcurrentHashMap<>());
        }
        Player newPlayer = Player.generatePlayer(uuid);
        newPlayer.setRoomRow(roomIdToRooms.get(roomId).size() / 2);
        newPlayer.setRoomCol(roomIdToRooms.get(roomId).size() / 2);
        roomIdToUuidToPlayer.get(roomId).put(uuid, newPlayer);

        sendNowData(session, roomId, uuid);
        System.out.println("房间号:" + roomId + ", 客户端：" + uuid + "建立连接" + ",当前玩家数量:" + roomIdToUuidToPlayer.get(roomId).size());
    }

    /**
     * 收到客户端消息后调用的方法
     *
     * @param message 客户端发送过来的消息
     */
    @OnMessage
    public void synchPlayers(Session session, String message, @PathParam("roomId") String roomId, @PathParam("uuid") String uuid) throws IOException {
//        System.out.println("收到来自客户端：" + uuid + "的信息:" + message);
        // 同步当前数据
        SyncDataVO syncDataVO = objectMapper.readValue(message, SyncDataVO.class);

//        System.out.println(syncDataVO.getRoom().getItems());
        Player player = syncDataVO.getPlayer();
        Player oldPlayer = roomIdToUuidToPlayer.get(roomId).get(uuid);
        Room room = syncDataVO.getRoom();
        System.out.println(player);
        if(player.getRoomCol() == oldPlayer.getRoomCol() && player.getRoomRow() == oldPlayer.getRoomRow()){
            roomIdToRooms.get(roomId).get(player.getRoomRow()).set(player.getRoomCol(), room);

        }else{
            System.out.println("下一个房间");
        }
        roomIdToUuidToPlayer.get(roomId).put(uuid, player);

        sendNowData(session, roomId, uuid);
    }

    private void sendNowData(Session session, String roomId, String uuid) throws IOException {
        // 发送当前数据
        Player nowPlayer = roomIdToUuidToPlayer.get(roomId).get(uuid);
        Room playerRoom = roomIdToRooms.get(roomId)
                .get(nowPlayer.getRoomRow())
                .get(nowPlayer.getRoomCol());
        List<Player> players = new ArrayList<>(roomIdToUuidToPlayer.get(roomId).values());
        SendDataVO data = SendDataVO.builder()
                .players(players)
                .room(playerRoom)
                .row(nowPlayer.getRoomRow())
                .col(nowPlayer.getRoomCol())
                .build();
        session.getBasicRemote().sendText(objectMapper.writeValueAsString(data));
    }

    /**
     * 连接关闭调用的方法
     *
     * @param uuid
     */
    @OnClose
    public void onClose(@PathParam("roomId") String roomId, @PathParam("uuid") String uuid) {
        System.out.println("连接断开:" + uuid);
//        sessionMap.remove(uuid);
        roomIdToUuidToPlayer.get(roomId).remove(uuid);
    }


    public void sendAllPlayers(Session session, ArrayList<Player> players) {
        try {
            session.getBasicRemote().sendText(objectMapper.writeValueAsString(Result.success(players)));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

}
