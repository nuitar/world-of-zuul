import { Scene } from "phaser";
import { Item } from "../entity/itmes";
import { Player } from "../entity/player";
import { components, rooms, players } from "../config/itemsConfig.json";

export class Game extends Scene {
    /**
     * @type {Player}
     */
    player;
    cursors;
    /**
     *  @type {Phaser.Physics.Arcade.StaticGroup}
     */
    boxes;
    constructor() {
        super("Game");
    }

    preload() {
        this.load.image("gem", "assets/gem.png");

        for (let i = 0; i < components.length; i++) {
            this.load.image(components[i].texture, components[i].path);
        }

        for (let i = 0; i < rooms.length; i++) {
            this.load.image(rooms[i].texture, rooms[i].path);
        }

        for (let i = 0; i < players.length; i++) {
            this.load.image(players[i].texture, players[i].path);
        }
    }

    create() {
        this.otherPlayers = new Map();

        this.add.image(512, 384, "room_01").setScale(0.8);
        this.physics.world.setBounds(130, 60, 780, 700);

        this.initGem();

        this.boxes = this.physics.add.staticGroup();
        this.boxes.create(200, 500, "silver_box").setScale(0.1).refreshBody();

        // this.player = this.physics.add.sprite(512, 384, "player_0");
        this.player = new Player(
            this,
            512,
            384,
            "player_0",
            this.generateUUID()
        );

        this.player.setCollideWorldBounds(true);
        this.player.setScale(0.1);
        this.physics.add.collider(
            this.player,
            this.boxes,
            this.openBox,
            null,
            this
        );
        this.loadConfigFile();
        this.initBag();
        this.initWebSocket();
        this.cursors = this.input.keyboard.createCursorKeys();
    }

    loadConfigFile() {
        console.log(components);
    }

    update() {
        this.keyboardMove();
        this.synchPlayer();
        // console.log(this.player.x, this.player.y);
    }

    openBox(player, box) {
        box.setTexture("silver_box_open"); // 更改平台的图片
    }

    initGem() {
        const text = this.add.text(100, 20, "", {
            font: "16px Courier",
            fill: "#00ff00",
        });
        const gem = this.add.image(70, 50, "gem");

        //  Store some data about this Gem:
        gem.setData({
            name: "Red Gem Stone",
            level: 2,
            owner: "Link",
            gold: 50,
        });

        //  Whenever a data value is updated the `changedata` event is fired and we listen for it:
        gem.on("changedata", function (gameObject, key, value) {
            text.setText([
                "Name: " + gem.getData("name"),
                "Level: " + gem.getData("level"),
                "Value: " + gem.getData("gold") + " gold",
                "Owner: " + gem.getData("owner"),
            ]);
        });

        //  Change the 'value' property when the mouse is clicked
        this.input.on("pointerdown", function () {
            gem.data.values.gold += 50;
            if (gem.data.values.gold % 200 === 0) {
                gem.data.values.level++;
            }
        });
    }

    initBag() {
        this.backpackButton = this.add
            .image(500, 500, "button")
            .setInteractive();
        this.backpackButton.setScale(0.5);
        // 创建背包界面
        this.backpackContainer = this.add.container(400, 300);
        this.backpackContainer.setVisible(false); // 初始状态隐藏

        let bg = this.add.rectangle(0, 0, 300, 300, 0xe8c5e5).setOrigin(0.5);
        this.backpackContainer.add(bg);

        const itemsJson = [
            {
                name: "item1",
                texture: "golden_key",
                count: 1,
                description: "可以打开黄金箱子",
            },
            {
                name: "item2",
                texture: "silver_key",
                count: 2,
                description: "可以打开银箱子",
            },
            {
                name: "item3",
                texture: "copper_key",
                count: 3,
                description: "可以打开铜箱子",
            },
        ];

        // 创建方格和物品
        let gridSize = 50;
        let gridRows = 5;
        let gridCols = 5;
        for (let i = 0; i <= gridRows; i++) {
            for (let j = 0; j <= gridCols; j++) {
                // 创建白色边框方格
                let x = -125 + j * gridSize;
                let y = -125 + i * gridSize;
                let slot = this.add
                    .rectangle(x, y, gridSize - 2, gridSize - 2)
                    .setStrokeStyle(2, 0xffffff);
                this.backpackContainer.add(slot);
            }
        }
        let items = [];

        itemsJson.forEach((itemData, index) => {
            let x = -125 + (index % gridCols) * gridSize;
            let y = -125 + Math.floor(index / gridCols) * gridSize;
            let item = new Item(
                this,
                this.backpackContainer,
                x,
                y,
                itemData.texture,
                itemData.name,
                itemData.count,
                itemData.description
            );
            items.push(item);
        });

        items.forEach((itemData, index) => {
            this.backpackContainer.bringToTop(itemData.descriptionText);
        });

        // 为按钮添加点击事件
        this.backpackButton.on("pointerdown", () => {
            this.backpackContainer.setVisible(!this.backpackContainer.visible);
        });
    }

    keyboardMove() {
        if (this.cursors.left.isDown) {
            this.player.setVelocityX(-160);
        } else if (this.cursors.right.isDown) {
            this.player.setVelocityX(160);
        } else {
            this.player.setVelocityX(0);
        }

        if (this.cursors.up.isDown) {
            this.player.setVelocityY(-160);
        } else if (this.cursors.down.isDown) {
            this.player.setVelocityY(160);
        } else {
            this.player.setVelocityY(0);
        }
    }
    initWebSocket() {
        const uuid = this.player.name;
        this.socket = new WebSocket("ws://localhost:7070/ws/" + uuid);
        this.socketConnect = false;

        this.socket.onopen = function (event) {
            console.log("WebSocket connection established.");
            this.socketConnect = true;
        }.bind(this);
        // 当接收到服务器消息时触发此事件
        this.socket.onmessage = function (event) {
            // console.log("Message from server: ", event.data);
            // this.otherPlayers = new Map();
            this.parseJsonToPlayers(event.data);
        }.bind(this);

        // 当连接关闭时触发此事件
        this.socket.onclose = function (event) {
            console.log("WebSocket connection closed: ", event);
        }.bind(this);

        // 当出现错误时触发此事件
        this.socket.onerror = function (error) {
            console.error("WebSocket error: ", error);
        }.bind(this);
    }

    parseJsonToPlayers(data) {
        let json_data = JSON.parse(event.data).data;
        // console.log(json_data);
        for (let i = 0; i < json_data.length; i++) {
            let player_json = json_data[i];
            if (player_json.name === this.player.name) continue;
            if (this.otherPlayers.has(player_json.name)) {
                this.otherPlayers
                    .get(player_json.name)
                    .setPosition(player_json.x, player_json.y);
                // console.log(this.otherPlayers.get(player_json.name));
            } else {
                this.otherPlayers.set(player_json.name, this.parseToSinglePlayer(player_json));
            }
        }
        if (json_data.length <= this.otherPlayers.size) {
            for (let [key, value] of this.otherPlayers) {
                if (!json_data.includes(value)) {
                    console.log("删除");
                    value.destroy();
                    this.otherPlayers.delete(key);
                }
            }
        }

    }

    parseToSinglePlayer(player_json) {
        let otherPlayer = new Player(
            this,
            512,
            384,
            player_json.texture,
            player_json.name
        );

        otherPlayer.setCollideWorldBounds(true);
        otherPlayer.setScale(0.1);
        this.physics.add.collider(
            otherPlayer,
            this.boxes,
            this.openBox,
            null,
            this
        );
        return otherPlayer;
    }
    synchPlayer() {
        console.log(this.otherPlayers.size);
        if (this.socketConnect) this.socket.send(this.playerToJson());
    }

    playerToJson() {
        let player = {
            x: this.player.x,
            y: this.player.y,
            name: this.player.name,
            texture: this.player.texture.key,
        };
        return JSON.stringify(player);
    }

    generateUUID() {
        let dt = new Date().getTime();
        let uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(
            /[xy]/g,
            function (c) {
                let r = (dt + Math.random() * 16) % 16 | 0;
                dt = Math.floor(dt / 16);
                return (c === "x" ? r : (r & 0x3) | 0x8).toString(16);
            }
        );
        return uuid;
    }
}
