package com.nuitar.service;

import com.nuitar.pojo.DTO.UserEnollDTO;
import com.nuitar.pojo.DTO.UserLoginDTO;
import com.nuitar.pojo.User;

public interface UserService {
    public User login(UserLoginDTO userLoginDTO);

    void addUser(UserEnollDTO userEnollDTO);
}
