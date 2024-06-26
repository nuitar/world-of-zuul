package com.nuitar.pojo;

import lombok.Builder;
import lombok.Data;
import org.springframework.ui.freemarker.FreeMarkerTemplateUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

@Data
public class Item {
    private int x;
    private int y;
    private String texture;
    private static String[] fileNames = new String[]{"copper", "golden", "silver"};

    public static Item generateRandomItem() {
        Random random = new Random();
        int i = random.nextInt(3);
        Item item = new Item();
        item.setTexture(fileNames[i] + "_box");
        item.setX(random.nextInt(829 - 199) + 199);
        item.setY(random.nextInt(839 - 229) + 229);
        return item;
    }

    public static List<Item> generateRandomItems() {
        List<Item> ans = new ArrayList<>();
        Random random = new Random();
        int itemCount = random.nextInt(5);
        for (int i = 0; i < itemCount; i++) {
            Item item = Item.generateRandomItem();
            ans.add(item);
        }
        return ans;
    }
}
