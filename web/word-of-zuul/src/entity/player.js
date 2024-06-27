export class Player extends Phaser.Physics.Arcade.Sprite {
    constructor(scene, x, y, texture,name) {
        super(scene, x, y, texture);
        this.name = name;
        // 添加到场景中
        scene.add.existing(this);
        scene.physics.add.existing(this);

        // 初始血量和魔力
        this.health = 200;
        this.mana = 100;
        this.maxWeight = 200;
        this.nowWeight = 0;
        this.vx = 160;
        this.vy = 160
    }
}
