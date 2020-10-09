// anim obj {frames:[], frameMs:-1}
Player = function (tile, anims) {

    //move these to physics proto
    this.pos = new vec2(0, 0)
    this.vel = new vec2(0, 0)
    this.speed = 0.5

    this.tile = tile
    this.anims = anims
    this.anim = null

    this.id = sprite(tile, 0, 0)

    this.move_to = function (x, y) {
        this.pos = new vec2(x, y)
        this.vel = new vec2(0, 0)
    }

    this.animate = function (animIndex) {
        this.animIndex = animIndex
        this.frame = 0
        this.startTime = new Date()
        if (this.animIndex < this.anims.length) {
            this.anim = this.anims[this.animIndex]
            this.tile = this.anim.frames[this.frame]
            frame(this.frame)
        }
    }

    this.on_frame = function (delta_sec) {
        now = new Date()
        elapsed = now - this.startTime

        if (this.anim && elapsed > this.anim.frameMs) {
            this.frame += 1
            this.frame %= this.anim.frames.length
            this.startTime = new Date()
            this.tile = this.anim.frames[this.frame]
            frame(this.tile)
        }
        vel = new vec2(this.vel.x * delta_sec, this.vel.y * delta_sec )

        this.pos = this.pos.add(this.vel)
        pos(this.id, this.pos.x, this.pos.y)
    }

    this.on_key = function (key, state) {
        switch (state) {
            case BTN_CLICK:
            case BTN_HOLD:
                switch (key) {
                    case KEY_w:
                        P0.vel.y = - 1 * P0.speed
                        break
                    case KEY_s:
                        P0.vel.y = P0.speed
                        break
                    case KEY_a:
                        P0.vel.x = -1 * P0.speed
                        break
                    case KEY_d:
                        P0.vel.x = P0.speed
                        break
                }
                break
            case BTN_RELEASE:
                switch (key) {
                    case KEY_w:
                        P0.vel.y = 0
                        break
                    case KEY_s:
                        P0.vel.y = 0
                        break
                    case KEY_a:
                        P0.vel.x = 0
                        break
                    case KEY_d:
                        P0.vel.x = 0
                        break
                }
                break
        }
    }
}