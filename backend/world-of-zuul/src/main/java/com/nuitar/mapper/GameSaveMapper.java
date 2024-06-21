package com.nuitar.mapper;

import com.nuitar.pojo.SaveGameUser;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface GameSaveMapper {
    @Insert("insert into game_save(user_id, data) values ")
     void saveGame(SaveGameUser saveGameUser);
}
