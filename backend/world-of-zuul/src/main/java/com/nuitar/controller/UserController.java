package com.nuitar.controller;

import com.nuitar.pojo.DTO.QuerySaveDTO;
import com.nuitar.pojo.DTO.QueryScoreDTO;
import com.nuitar.pojo.DTO.UserEnollDTO;
import com.nuitar.pojo.DTO.UserLoginDTO;
import com.nuitar.pojo.UserScore;
import com.nuitar.pojo.SaveGameUser;
import com.nuitar.pojo.User;
import com.nuitar.service.UserService;
import com.nuitar.utils.Result;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.ArrayList;
import java.util.List;

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

    @PostMapping("/save")
    public Result saveData(@RequestBody SaveGameUser saveGameUser){
        userService.saveData(saveGameUser);
        return Result.success();
    }

//    @PostMapping("/save/{userId}")
//    public Result getData(@PathVariable int userId){
//        SaveGameUser saveGameUser = userService.queryData(userId);
//
//        return Result.success(saveGameUser);
//    }

    @PostMapping("/save/query")
    public Result getData(@RequestBody QuerySaveDTO querySaveDTO){
        SaveGameUser saveGameUser = userService.queryData(querySaveDTO.getUserId());
        return Result.success(saveGameUser);
    }
    @PostMapping("/socre")
    public Result addScore(@RequestBody UserScore userScore){
        userService.addScore(userScore);
        return Result.success();
    }

//    @PostMapping("/socre/{userId}")
//    public Result queryScore(@PathVariable int userId){
//        List<UserScore> userScores =  userService.queryScore(userId);
//        return Result.success(userScores);
//    }
    @PostMapping("/socre/query")
    public Result queryScore(@RequestBody QueryScoreDTO queryScoreDTO){
        List<UserScore> userScores =  userService.queryScore(queryScoreDTO.getUserId());
        return Result.success(userScores);
    }
}

