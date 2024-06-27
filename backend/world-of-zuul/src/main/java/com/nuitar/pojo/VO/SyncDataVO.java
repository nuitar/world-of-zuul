package com.nuitar.pojo.VO;

import com.nuitar.pojo.Player;
import com.nuitar.pojo.Room;
import lombok.Builder;
import lombok.Data;

@Data
public class SyncDataVO {
    private Room room;
    private Player player;
}
