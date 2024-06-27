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
    private static String[] fileNames = new String[]{"copper_box", "golden_box", "silver_box", "hypno_shroom", "lightning_boot", "magic_cookie"};

    public static Item generateRandomItem() {
        Random random = new Random();
        int i = random.nextInt(fileNames.length);
        Item item = new Item();
        item.setTexture(fileNames[i]);
        item.setX(random.nextInt(829 - 199) + 199);
        item.setY(random.nextInt(839 - 229) + 229);
        return item;
    }

    public static boolean checkCollide(Item item1, Item item2) {
        return Math.abs(item1.x - item2.x) < 50 || Math.abs(item1.y - item2.y) < 50;
    }

    public static boolean checkAnyCollide(List<Item>items,Item item){
        for (int i = 0; i < items.size(); i++) {
            if(checkCollide(items.get(i),item))
                return true;
        }
        return false;
    }
    public static List<Item> generateRandomItems() {
        List<Item> ans = new ArrayList<>();
        Random random = new Random();
        int itemCount = random.nextInt(7) + 2;
        for (int i = 0; i < itemCount; i++) {
            Item item = generateRandomItem();
            while(checkAnyCollide(ans,item))
                item = generateRandomItem();
            ans.add(item);
        }
        return ans;
    }
}
