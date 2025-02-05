import { Scene } from "phaser";

export class MainMenu extends Scene {
    constructor() {
        super("MainMenu");
    }
    preload() {
        this.load.html("login", "assets/html/enroll.html");
    }
    addText(x, y, text, next_scene) {
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
        action.setInteractive({ useHandCursor: true });
        action.on("pointerdown", () => {
            this.scene.start(next_scene);
        });
    }

    initMenu() {
        const canvasWidth = this.game.canvas.width;
        const canvasHeight = this.game.canvas.height;
        let menus = ["开始游戏", "创建房间", "游戏设置", "关于我们"];
        let scenes = ["Game", "CreateRoom", "Setting", "About"];
        for (let i = 0; i < menus.length; i++) {
            this.addText(
                canvasWidth / 2,
                canvasHeight / 2 + i * 100,
                menus[i],
                scenes[i]
            );
        }
    }

    create() {
        this.add.image(512, 512, "background");

        this.add.image(512, 300, "logo");

        this.initMenu();
        // this.initForm();
    }

    initForm() {
        const element = this.add.dom(512, 384).createFromCache("login");

        element.setPerspective(800);

        element.addListener("click");

        element.on("click", function (event) {
            if (event.target.name === "createRoomButton") {
                const roomId = this.getChildByName("roomId");
                if (roomId.value !== "") {
                    let roomIdValue = roomId.value;
                    this.removeListener("click");
                    element.setVisible(false);
                    this.scene.initWebSocket(roomIdValue);
                    this.scene.gem.data.values.roomId = roomIdValue;
                }
            }
            if (event.target.name === "quitButton") {
                this.removeListener("click");
                element.setVisible(false);
            }
        });
    }
}
