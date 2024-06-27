import { Scene } from "phaser";
import Slider from "phaser3-rex-plugins/plugins/slider.js";

export class Setting extends Scene {
    constructor() {
        super("Setting");
    }
    preload() {
        this.load.image("thumb", "assets/key/copper_key.png");

        this.load.scenePlugin({
            key: "rexuiplugin",
            url: "https://raw.githubusercontent.com/rexrainbow/phaser3-rex-notes/master/dist/rexuiplugin.min.js",
            sceneKey: "rexUI",
        });
    }

    create() {
        let container = this.add.container(400, 300);
        var slider = new Slider(this.game, {
            x: 400,
            y: 300,
            width: 200,
            height: 20,
            orientation: "x", // 水平滑块，'y' 为垂直滑块

            track: this.add.image(0, 0, "background"),
            thumb: this.add.image(0, 0, "thumb"),

            value: 0, // 初始值
            gap: 0.1, // 步进值

            valuechangeCallback: function (value) {
                console.log("Slider value: " + value);
            },

            space: {
                top: 4,
                bottom: 4,
                left: 4,
                right: 4,
            },
        });
    }

    update() {
        // 可以在这里添加任何需要的更新逻辑
    }
}
