package com.nuitar;

import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
class WorldOfZuulApplicationTests {

    @Test
    void contextLoads() {
        System.out.println(1);
    }

}
