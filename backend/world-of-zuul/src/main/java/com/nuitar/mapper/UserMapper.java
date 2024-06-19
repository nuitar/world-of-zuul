package com.nuitar.mapper;

import com.nuitar.pojo.User;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Select;

@Mapper
public interface UserMapper {

    @Select("select * from user where username = #{username}")
    User getByUsername(String username);

    @Insert("insert into user(username, password) values (#{username},#{password});")
    void add(User user);
}

