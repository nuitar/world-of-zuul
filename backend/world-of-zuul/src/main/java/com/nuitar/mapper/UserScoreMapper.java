package com.nuitar.mapper;

import com.nuitar.pojo.UserScore;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Select;

import java.util.List;

@Mapper
public interface UserScoreMapper {
    @Insert("insert into user_score(user_id, game_history_data, time) values (#{userId},#{gameHistoryData},#{time})")
    void add(UserScore userScore);

    @Select("select * from user_score where user_id = #{userId}")
    List<UserScore> getByUserId(int userId);
}
