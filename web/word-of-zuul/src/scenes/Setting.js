import { Scene } from "phaser";
import Slider from "phaser3-rex-plugins/plugins/slider.js";

export class Setting extends Scene {
    constructor() {
        super("Setting");
    }
    preload() {
        this.load.image("thumb", "assets/key/copper_key.png");
        this.load.audio("canon", ["assets/audio/canon-dylanf.mp3"]);

        this.load.html("gameSetting", "assets/html/gameSetting.html");
    }

    create() {
        this.input.on("pointerdown", () => {
            this.scene.start("MainMenu");
            this.sound.stopAll();
        });
        this.initForm();
        if (this.sound.getAllPlaying().length === 0) {
            const music = this.sound.add("canon");
            music.setVolume(0.25);
            music.play();
        }

        this.addText(280, 315, "音乐大小");
        this.addText(280, 380, "人物移速");
        this.addText(280, 450, "背包负重");

        this.musicVolmeText = this.addText(730, 315, "25");
        this.moveText = this.addText(730, 380, "160");
        this.weightText = this.addText(730, 450, "200");

        
    }

    update() {
        // 可以在这里添加任何需要的更新逻辑
    }
    addText(x, y, text) {
        let action = this.add
            .text(x, y, text, {
                fontFamily: "Arial Black",
                fontSize: 25,
                color: "#ffffff",
                stroke: "#000000",
                strokeThickness: 5,
                align: "center",
            })
            .setOrigin(0.5);
        return action;
    }

    initForm() {
        const element = this.add.dom(512, 384).createFromCache("gameSetting");

        element.setPerspective(800);

        element.addListener("input");

        element.on("input", function (event) {
            if (event.target.name === "musicRange") {
                const musicRange = this.getChildByName("musicRange");
                console.log(musicRange.value);
                this.scene.sound.setVolume(musicRange.value / 100);
                this.scene.musicVolmeText.setText(musicRange.value);
            }
            else if (event.target.name === "moveRange") {
                this.scene.moveText.setText(event.target.value);
            } else if (event.target.name === "weightRange") {
                this.scene.weightText.setText(event.target.value); 
            }
        });
    }
}
