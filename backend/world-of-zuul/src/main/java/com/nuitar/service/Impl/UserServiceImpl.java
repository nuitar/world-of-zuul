package com.nuitar.service.Impl;

import com.nuitar.exception.BaseException;
import com.nuitar.mapper.GameSaveMapper;
import com.nuitar.pojo.DTO.UserEnollDTO;
import com.nuitar.pojo.DTO.UserLoginDTO;
import com.nuitar.pojo.SaveGameUser;
import com.nuitar.pojo.User;
import com.nuitar.mapper.UserMapper;
import com.nuitar.service.UserService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
@Slf4j
public class UserServiceImpl implements UserService {
    @Autowired
    UserMapper userMapper;

    @Autowired
    GameSaveMapper gameSaveMapper;
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
        gameSaveMapper.saveGame(saveGameUser);
    }
}
