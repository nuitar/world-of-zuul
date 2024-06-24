import { Scene } from "phaser";

export class Item extends Phaser.GameObjects.Image {
    /**
     * @param {Scene} scene 第一个数字
     */
    constructor(scene, container, x, y, texture, name, count, description) {
        super(scene, x, y, texture);
        this.scene = scene;
        this.container = container;
        this.name = name;
        this.count = count;
        this.description = description;

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
                fill: "#ffffff",
            })
            .setOrigin(0.5);

        // 添加描述文本（初始状态隐藏）
        this.descriptionText = this.scene.add
            .text(x, y - 20, this.description, {
                fontSize: "12px",
                fill: "#ffffff",
                backgroundColor: "#000000",
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
            if (!this.useButton.visible)
                this.showButtons();
            else
                this.hideButtons();
        });

        this.useButton.on("pointerdown", () => {
            this.use();
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

    use() {
        this.updateCount(this.count - 1);
        this.hideButtons();
        // TODO 物品使用逻辑
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
        this.remove();
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
        this.count = newCount;
        if (this.count <= 0) {
            this.remove();
        }
        this.countText.setText(newCount);
    }
}
