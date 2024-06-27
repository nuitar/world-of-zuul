export class Room extends Phaser.GameObjects.Image{
    constructor(scene, container, x, y, texture, name, count, description) {
        super(scene, x, y, texture);
        this.scene = scene;
        this.container = container;
        this.name = name;
        this.count = count;
        this.description = description;
        
    }
}