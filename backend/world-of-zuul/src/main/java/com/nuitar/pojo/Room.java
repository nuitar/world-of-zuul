package com.nuitar.pojo;

import lombok.Builder;
import lombok.Data;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

@Data
public class Room {
    private String texture;
    private List<Item> items;

    public static Room generateRandomRoom(){
        Random random = new Random();
        int randomNumber = random.nextInt(9) + 1;
        Room room = new Room();
        room.setItems(Item.generateRandomItems());
        room.setTexture("room_0" + randomNumber);
        return room;
    }
    public static List<List<Room>> generateRooms(int row,int col){
        List<List<Room>> rooms = new ArrayList<>();
        for (int i = 0; i < row; i++) {
            ArrayList<Room> rowRooms = new ArrayList<>();
            for (int j = 0; j < col; j++) {
                rowRooms.add(generateRandomRoom());
            }
            rooms.add(rowRooms);
        }
        return rooms;
    }
}
