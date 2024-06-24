package com.nuitar.pojo;

import lombok.Data;

import java.util.ArrayList;
import java.util.List;

@Data
public class Player {
    private String name;
    private String texture;

    private Double x;
    private Double y;
    private Integer roomX;
    private Integer roomY;
}
