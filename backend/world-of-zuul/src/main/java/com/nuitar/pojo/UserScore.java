package com.nuitar.pojo;


import lombok.Data;

import java.time.LocalDateTime;

@Data
public class UserScore {
    private int id;
    private int userId;
    private String gameHistoryData;
    private LocalDateTime time;
}
