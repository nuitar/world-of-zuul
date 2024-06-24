package com.nuitar.service;

import com.nuitar.pojo.DTO.UserEnollDTO;
import com.nuitar.pojo.DTO.UserLoginDTO;
import com.nuitar.pojo.SaveGameUser;
import com.nuitar.pojo.User;
import com.nuitar.pojo.UserScore;

import java.util.List;

public interface UserService {
    public User login(UserLoginDTO userLoginDTO);

    void addUser(UserEnollDTO userEnollDTO);

    void saveData(SaveGameUser saveGameUser);

    SaveGameUser queryData(Integer userId);

    void addScore(UserScore userScore);


    List<UserScore> queryScore(int userId);
}
