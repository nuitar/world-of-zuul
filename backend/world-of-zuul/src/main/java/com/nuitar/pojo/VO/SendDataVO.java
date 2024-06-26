package com.nuitar.pojo.VO;

import com.nuitar.pojo.Player;
import com.nuitar.pojo.Room;
import lombok.Builder;
import lombok.Data;

import java.util.List;

@Data
@Builder
public class SendDataVO {
    private Room room;
    private List<Player> players;
    private int row;
    private int col;
}
