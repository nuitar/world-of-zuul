package com.nuitar.mapper;

import com.nuitar.pojo.SaveGameUser;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Select;
import org.apache.ibatis.annotations.Update;

@Mapper
public interface GameSaveMapper {
    @Insert("insert into game_save(user_id, save_game_data) values (#{userId},#{saveGameData})")
     void saveGame(SaveGameUser saveGameUser);

    @Select("select * from game_save where user_id = #{userId}")
    SaveGameUser getByUserId(Integer userId);
    @Update("update game_save set save_game_data = #{saveGameData} where user_id = #{userId}")
    void updateSave(SaveGameUser saveGameUser);
}
