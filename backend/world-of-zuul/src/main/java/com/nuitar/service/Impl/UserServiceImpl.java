package com.nuitar.service.Impl;

import com.nuitar.exception.BaseException;
import com.nuitar.mapper.GameSaveMapper;
import com.nuitar.mapper.UserScoreMapper;
import com.nuitar.pojo.DTO.UserEnollDTO;
import com.nuitar.pojo.DTO.UserLoginDTO;
import com.nuitar.pojo.SaveGameUser;
import com.nuitar.pojo.User;
import com.nuitar.mapper.UserMapper;
import com.nuitar.pojo.UserScore;
import com.nuitar.service.UserService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;

@Service
@Slf4j
public class UserServiceImpl implements UserService {
    @Autowired
    UserMapper userMapper;

    @Autowired
    GameSaveMapper gameSaveMapper;

    @Autowired
    UserScoreMapper userScoreMapper;
    @Override
    public User login(UserLoginDTO userLoginDTO) {
        User user = userMapper.getByUsername(userLoginDTO.getUsername());
        if (user == null) {
            throw new BaseException("账号不存在");
        } else if (!user.getPassword().equals(userLoginDTO.getPassword())) {
            throw new BaseException("密码错误");
        }
        return user;
    }

    @Override
    public void addUser(UserEnollDTO userEnollDTO) {
        User user = new User();
        user.setUsername(userEnollDTO.getUsername());
        user.setPassword(userEnollDTO.getPassword());

        userMapper.add(user);
    }

    @Override
    public void saveData(SaveGameUser saveGameUser) {
        SaveGameUser user = gameSaveMapper.getByUserId(saveGameUser.getUserId());
        if(user == null){
            gameSaveMapper.saveGame(saveGameUser);

        }else{
            gameSaveMapper.updateSave(saveGameUser);
        }
    }

    @Override
    public SaveGameUser queryData(Integer userId) {
        SaveGameUser user = gameSaveMapper.getByUserId(userId);
        return user;
    }

    @Override
    public void addScore(UserScore userScore) {
        userScore.setTime(LocalDateTime.now());
        userScoreMapper.add(userScore);
    }

    @Override
    public List<UserScore> queryScore(int userId) {
        List<UserScore> userScores = userScoreMapper.getByUserId(userId);
        return userScores;
    }



}
