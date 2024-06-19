package com.nuitar.controller;

import com.nuitar.pojo.DTO.UserEnollDTO;
import com.nuitar.pojo.DTO.UserLoginDTO;
import com.nuitar.pojo.User;
import com.nuitar.service.UserService;
import com.nuitar.utils.Result;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/user")
public class UserController {
    @Autowired
    UserService userService;

    @PostMapping("/login")
    public Result login(@RequestBody UserLoginDTO userLoginDTO){
        User user = userService.login(userLoginDTO);
        return Result.success(user);
    }

    @PostMapping("/enroll")
    public Result addUser(@RequestBody UserEnollDTO userEnollDTO){
        userService.addUser(userEnollDTO);
        return Result.success();
    }
}

