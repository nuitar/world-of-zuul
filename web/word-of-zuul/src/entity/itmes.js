import { Scene } from "phaser";

export class BagItem extends Phaser.GameObjects.Image {
    /**
     * @param {Scene} scene
     */
    constructor(
        scene,
        player,
        container,
        x,
        y,
        texture,
        name,
        weight,
        count,
        description
    ) {
        super(scene, x, y, texture);
        // this.scene = scene;
        this.weight = weight;
        this.player = player;
        this.container = container;
        this.name = name;
        this.count = count;
        this.description = description;

        this.player.nowWeight += count*weight;

        this.setDisplaySize(40, 40);

        this.initComponent(x, y);

        this.container.add(this);
        this.container.add(this.countText);
        this.container.add(this.descriptionText);
        this.container.add(this.useButton);
        this.container.add(this.discardButton);
        this.container.add(this.discardAllButton);

        this.initEvent();
    }

    initComponent(x, y) {
        // 添加使用按钮
        this.useButton = this.scene.add
            .text(x + 10, y + 10, "使用", {
                fontSize: "12px",
                fill: "#ffffff",
                backgroundColor: "#000000",
            })
            .setOrigin(0.5)
            .setVisible(false)
            .setInteractive();

        // 添加丢弃按钮
        this.discardButton = this.scene.add
            .text(x + 10, y + 25, "丢弃", {
                fontSize: "12px",
                fill: "#ffffff",
                backgroundColor: "#000000",
            })
            .setOrigin(0.5)
            .setVisible(false)
            .setInteractive();

        this.discardAllButton = this.scene.add
            .text(x + 10, y + 40, "丢弃全部", {
                fontSize: "12px",
                fill: "#ffffff",
                backgroundColor: "#000000",
            })
            .setOrigin(0.5)
            .setVisible(false)
            .setInteractive();

        // 添加物品数量文本
        this.countText = this.scene.add
            .text(x + 10, y + 10, this.count, {
                fontSize: "16px",
                fill: "#000000",
            })
            .setOrigin(0.5);

        // 添加描述文本（初始状态隐藏）
        this.descriptionText = this.scene.add
            .text(x, y - 20, this.description + "\n" + "weight:" + this.weight, {
                fontSize: "12px",
                fill: "#000000",
                backgroundColor: "#ffffff",
            })
            .setOrigin(0.5)
            .setVisible(false);
    }

    initEvent() {
        // 设置交互
        this.setInteractive();

        // 添加鼠标悬停事件
        this.on("pointerover", () => {
            this.descriptionText.setVisible(true);
        });

        // 添加鼠标移出事件
        this.on("pointerout", () => {
            this.descriptionText.setVisible(false);
        });

        this.on("pointerdown", () => {
            if (!this.useButton.visible) this.showButtons();
            else this.hideButtons();
        });

        this.useButton.on("pointerdown", () => {
            this.use(this.scene, this.player);
            console.log(this.name + "使用成功");
        });

        this.discardButton.on("pointerdown", () => {
            this.discard();
            console.log(this.name + "丢弃成功");
        });

        this.discardAllButton.on("pointerdown", () => {
            this.discardAll();
            console.log(this.name + "丢弃成功");
        });
    }

    use(scene, player) {
        this.updateCount(this.count - 1);
        this.hideButtons();
        // TODO 物品使用逻辑
        let textureKey = this.texture.key;
        let v = 80;
        if (textureKey.includes("box")) {
        } else if (textureKey.includes("hypno_shroom")) {
            player.vx += v;
            player.vy += v;

            this.playerVReverse(player);

            scene.time.addEvent({
                delay: 5000,
                callback: () => {
                    // this.playerVReverse(player)
                    player.vx = 160;
                    player.vy = 160;
                },
                // callbackScope: this.scene,
            });
        } else if (textureKey.includes("lightning_boot")) {
            player.vx += v;
            player.vy += v;

            scene.time.addEvent({
                delay: 5000,
                callback: () => {
                    // this.playerVReverse(player)
                    player.vx = 160 - v * 0.5;
                    player.vy = 160 - v * 0.5;
                },
                // callbackScope: this.scene,
            });
            scene.time.addEvent({
                delay: 8000,
                callback: () => {
                    player.vx = 160;
                    player.vy = 160;
                },
                // callbackScope: this.scene,
            });
        } else if (textureKey.includes("magic_cookie")) {
            player.maxWeight += 50;
        }
        // if (!textureKey.includes("box")) {
        //     this.player.nowWeight -= this.weight;
        // }
    }

    playerVReverse(player) {
        player.vx *= -1;
        player.vy *= -1;
    }

    showButtons() {
        this.useButton.setVisible(true);
        this.discardButton.setVisible(true);
        this.discardAllButton.setVisible(true);
    }

    // 隐藏按钮
    hideButtons() {
        this.useButton.setVisible(false);
        this.discardButton.setVisible(false);
        this.discardAllButton.setVisible(false);
    }
    discardAll() {
        this.updateCount(0);
    }

    discard() {
        this.updateCount(this.count - 1);
        this.hideButtons();
    }

    remove() {
        this.container.remove(this);
        this.container.remove(this.descriptionText);
        this.container.remove(this.countText);
        this.container.remove(this.useButton);
        this.container.remove(this.discardButton);
        this.container.remove(this.discardAllButton);

        this.destroy();
    }

    // 更新物品数量
    updateCount(newCount) {
        let diff = newCount - this.count;

        this.count = newCount;
        this.player.nowWeight += diff*this.weight;

        if (this.count <= 0) {
            this.remove();
        }
        this.countText.setText(newCount);
    }
}

export class RoomItem extends Phaser.GameObjects.Image {}
