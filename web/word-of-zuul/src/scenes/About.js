import { Scene } from "phaser";

export class About extends Scene {
    constructor() {
        super("About");
    }

    preload() {
        //  The Boot Scene is typically used to load in any assets you require for your Preloader, such as a game logo or background.
        //  The smaller the file size of the assets, the better, as the Boot Scene itself has no preloader.

        this.load.image("background", "assets/bg.png");
        this.load.audio("canon", ["assets/audio/canon-dylanf.mp3"]);
    }

    create() {
        this.input.on("pointerdown", () => {
            this.scene.start("MainMenu");
            this.sound.stopAll();
        });
        if (this.sound.getAllPlaying().length === 0) {
            const music = this.sound.add("canon");
            music.setVolume(0.25);
            music.play();
        }
        let t =
            "游戏策划\n张烜 宋俊康 王子奕\n\n" +
            "游戏设计\n张烜 宋俊康 王子奕\n\n" +
            "程序开发\n宋俊康 王子奕 张烜\n\n" +
            "游戏测试\n宋俊康 张烜 王子奕\n\n" +
            "美术设计\n张烜\n\n音乐\n张烜\n\n" +
            "网页开发\n宋俊康\n\n" +
            "客户端开发\n王子奕\n\n" +
            "后端开发\n宋俊康";

        let text = this.addText(512, 1500, t);

        // 使用 tween 来控制文本对象的滚动
        this.tweens.add({
            targets: text,
            y: -text.height, // 目标位置，超出屏幕顶部
            duration: 14000, // 滚动持续时间，单位毫秒
            ease: "Linear", // 使用线性缓动函数
            repeat: -1, // 无限重复
            onRepeat: function () {
                // 每次重复时重置文本对象的位置
                text.y = 1500;
            },
        });
    }

    update() {}

    addText(x, y, text) {
        let action = this.add
            .text(x, y, text, {
                fontFamily: "Arial Black",
                fontSize: 38,
                color: "#ffffff",
                stroke: "#000000",
                strokeThickness: 8,
                align: "center",
            })
            .setOrigin(0.5);
        return action;
    }
}
