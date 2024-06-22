package com.nuitar.pojo.VO;

import lombok.Data;

import java.time.LocalDateTime;

@Data
public class UserScoreVO {
    private int id;
    private int userId;
    private String gameHistoryData;
    private LocalDateTime time;
}
