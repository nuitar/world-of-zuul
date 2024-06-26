import { NONE, Scene } from "phaser";
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
    row;
    col;
    constructor() {
        super("Game");
        this.row = -1;
        this.col = -1;
        this.i = 0;
    }

    preload() {
        this.load.image("gem", "assets/gem.png");
        this.load.html("nameform", "assets/loginform.html");
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
        this.initForm();

        this.otherPlayers = new Map();

        this.initRoom();
        this.initGem();

        this.boxes = this.physics.add.staticGroup();
        this.boxes.create(200, 500, "silver_box").setScale(0.1).refreshBody();
        let uid = this.generateUUID();
        this.gem.data.values.uid = uid;
        // this.player = this.physics.add.sprite(512, 384, "player_0");
        this.player = new Player(this, 512, 384, "player_0", uid);

        this.player.setCollideWorldBounds(true);
        this.player.setScale(0.1);
        this.physics.add.collider(
            this.player,
            this.boxes,
            this.openBox,
            null,
            this
        );

        this.physics.world.setBounds(0, 0, 1024, 1024);
        this.cameras.main.setBounds(0, 0, 1024, 1024);
        this.cameras.main.startFollow(this.player, true, 0.05, 0.05);

        this.loadConfigFile();
        this.initBag();
        this.cursors = this.input.keyboard.createCursorKeys();
    }

    loadConfigFile() {
        console.log(components);
    }

    update() {
        this.keyboardMove();
        this.synchPlayer();
        this.nextRoom(this.player.x, this.player.y);
        // this.updateRainbowText();
        console.log(this.player.x, this.player.y);
    }

    nextRoom(x, y) {
        /**
         * 447.6500000000004 190.84999999999883
575.6500000000015 190.84999999999883

左边 146.31666666666683 604.183333333333
146.31666666666683 470.84999999999945

下边
426.316666666667 865.5166666666627
580.9833333333347 865.5166666666627

右边
858.3166666666641 598.8499999999998
858.3166666666641 476.1833333333328
         */
        let flag = false;
        if (x > 447 && x < 575 && y < 190) {
            this.row -= 1;
            flag = true;
            this.gem.data.values.row -= 1;
        } else if (x < 146 && y > 470 && y < 604) {
            this.col -= 1;
            flag = true;
            this.gem.data.values.col -= 1;
        } else if (x > 426 && x < 580 && y > 865) {
            this.row += 1;
            flag = true;
            this.gem.data.values.row += 1;
        } else if (x > 858 && y > 476 && y < 598) {
            this.col += 1;
            flag = true;
            this.gem.data.values.col += 1;
        }
        if (flag) {
            this.player.setPosition(512, 512);
            for (let [key, value] of this.otherPlayers) {
                value.destroy();
                this.otherPlayers.delete(key);
            }
        }
    }

    openBox(player, box) {
        if (!box.texture.key.includes("_open")) {
            box.setTexture(box.texture.key + "_open"); // 更改平台的图片
        }
    }

    initForm() {
        const element = this.add.dom(512, 384).createFromCache("nameform");

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
    initRoom() {
        let init_x = 512,
            init_y = 512;

        this.room = this.add.image(0, 0, "room_01").setOrigin(0);
        // this.physics.world.setBounds(235, 109, 556, 520);
        this.newRoom();
    }

    newRoom() {
        const randomIndex = Math.floor(Math.random() * rooms.length);

        // 获取随机元素
        const room = rooms[randomIndex];
        this.room.setTexture(room.texture);
    }

    initGem() {
        const text = this.add.text(100, 20, "", {
            font: "20px Courier",
            fill: "#00ff00",
        });
        this.hsv = Phaser.Display.Color.HSVColorWheel();
        this.roomLocationText = this.add.text(460, 512, "", {
            font: "40px Courier",
            fill: "#fff",
        });
        this.roomLocationText.setStroke('#fff', 16);
        this.roomLocationText.setShadow(2, 2, "#333333", 2, true, true);


        const gem = this.add.image(70, 50, "gem");

        //  Store some data about this Gem:
        gem.setData({
            uid: "Red Gem Stone",
            roomId: 2,
            owner: "Link",
        });

        //  Whenever a data value is updated the `changedata` event is fired and we listen for it:
        gem.on("changedata", function (gameObject, key, value) {
            text.setText([
                "UID：" + gem.getData("uid"),
                "当前房间号：" + gem.getData("roomId"),
                "当前负重：10",
                // "Owner: " + gem.getData("owner"),
            ]);
        });
        this.gem = gem;
        //  Change the 'value' property when the mouse is clicked
        // this.input.on("pointerdown", function () {
        //     gem.data.values.gold += 50;
        //     if (gem.data.values.gold % 200 === 0) {
        //         gem.data.values.level++;
        //     }
        // });
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
    initWebSocket(roomIdValue) {
        const uuid = this.player.name;
        this.socket = new WebSocket(
            "ws://localhost:7070/ws/" + roomIdValue + "/" + uuid
        );
        this.socketConnect = false;

        this.socket.onopen = function (event) {
            console.log("WebSocket connection established.");
        }.bind(this);

        // 当接收到服务器消息时触发此事件
        this.socket.onmessage = function (event) {
            // console.log("Message from server: ", event.data);
            // this.otherPlayers = new Map();
            this.parseSyncJson(JSON.parse(event.data));
            this.socketConnect = true;
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

    parseSyncJson(json) {
        // if (json.row != this.row || json.col != this.col) {
        //
        // }
        if (this.row == -1 && this.col == -1) {
            this.row = json.row;
            this.col = json.col;
        }
        this.roomLocationText.setText("(" + json.row + "," + json.col + ")");
        this.boxes.clear(true, true);

        // console.log(json)
        this.parseJsonUpdatePlayers(json.players);
        this.parseJsonUpdateRoomsItems(json.room);
    }

    parseJsonUpdateRoomsItems(json_data) {
        this.room.setTexture(json_data.texture);
        // console.log(json_data.texture);
        // console.log(this.room.texture.key);

        let items = json_data.items;
        console.log(items);
        for (let i = 0; i < items.length; i++) {
            let box_json = items[i];
            const specificBox = this.boxes
                .getChildren()
                .find((box) => box.x === box_json.x && box.y === box_json.y);
            if (specificBox) {
                specificBox.setTexture(box_json.texture);
                // console.log("更新");
            } else {
                // console.log(box_json);

                this.boxes
                    .create(box_json.x, box_json.y, box_json.texture)
                    .setScale(0.1)
                    .refreshBody();
            }
        }
    }

    parseJsonUpdatePlayers(json_data) {
        // console.log(json_data);
        for (let i = 0; i < json_data.length; i++) {
            let player_json = json_data[i];
            if (player_json.name === this.player.name) {
                this.player.setTexture(player_json.texture);
                continue;
            }

            if (this.otherPlayers.has(player_json.name)) {
                if (
                    player_json.roomRow != this.row ||
                    player_json.roomCol != this.col
                ) {
                    this.otherPlayers.get(player_json.name).setVisible(false);
                } else {
                    this.otherPlayers
                        .get(player_json.name)
                        .setPosition(player_json.x, player_json.y)
                        .setVisible(true);
                }

                // console.log(this.otherPlayers.get(player_json.name));
            } else {
                this.otherPlayers.set(
                    player_json.name,
                    this.parseToSinglePlayer(player_json)
                );
            }
        }
    }

    parseToSinglePlayer(player_json) {
        let otherPlayer = new Player(
            this,
            player_json.x,
            player_json.y,
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
        // console.log(this.otherPlayers.size);
        let sendJs = this.toSyncJson();
        // console.log(sendJs);
        if (this.socketConnect) this.socket.send(sendJs);
    }

    toSyncJson() {
        let json = {
            room: this.getRoom(),
            player: this.getPlayer(),
        };
        return JSON.stringify(json);
    }

    getRoom() {
        let room = {
            texture: this.room.texture.key,
            items: this.getBoxes(),
        };
        return room;
    }

    getBoxes() {
        let boxes = [];
        this.boxes.getChildren().forEach((box) => {
            const boxJson = {
                x: box.x,
                y: box.y,
                texture: box.texture.key,
            };
            boxes.push(boxJson);
        });
        return boxes;
    }

    getPlayer() {
        let player = {
            x: this.player.x,
            y: this.player.y,
            name: this.player.name,
            texture: this.player.texture.key,
            roomRow: this.row,
            roomCol: this.col,
        };
        return player;
    }

    updateRainbowText() {
        const top = this.hsv[this.i].color;
        const bottom = this.hsv[359 - this.i].color;

        this.roomLocationText.setTint(top, top, bottom, bottom);

        this.i++;

        if (this.i === 360)
        {
            this.i = 0;
        }
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
