package com.nuitar.pojo;

import lombok.Builder;
import lombok.Data;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

@Data
public class Player {
    private String name;
    private String texture;

    private Double x;
    private Double y;
    private Integer roomRow;
    private Integer roomCol;

    public static Player generatePlayer(String uuid) {
        Random random = new Random();

        Player player = new Player();

        player.setTexture("player_" + random.nextInt(2));
        player.setX(0.0);
        player.setY(0.0);
        player.setRoomRow(0);
        player.setRoomCol(0);
        player.setName(uuid);
        return player;
    }
}
