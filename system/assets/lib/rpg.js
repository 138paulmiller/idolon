// anim obj {frames:[], duration:-1}

Sprite = function (tile ) {

    //move these to physics proto
    this.pos = new vec2(0, 0)
    this.vel = new vec2(0, 0)
    this.speed = 0.25
    this.deccel = 0.5
    this.max_vel = 1
    this.is_moving = false

    this.frame_timer = 0
    this.frame = 0 //index in animation
    this.tile = tile
    this.anims = {} 
    this.cur_anim = null
    this.id = sprite(tile, 0, 0)
    this.size = new vec2(spr_w(this.id), spr_h(this.id))

    this.x = function () {
        return this.pos.x
    }

    this.y = function () {
        return this.pos.y
    }

    this.w = function () {
        return this.size.x
    }

    this.h = function () {
        return this.size.y
    }

    this.move_by = function (dx, dy) {
        this.vel.x += dx * this.speed
        this.vel.y += dy * this.speed
        this.vel.x = clamp(this.vel.x, -this.max_vel, this.max_vel)
        this.vel.y = clamp(this.vel.y, -this.max_vel, this.max_vel)
    }

    this.move_to = function (x, y) {
        this.pos = new vec2(x, y)
        this.vel = new vec2(0, 0)
    }

    this.kill = function () {
        kill( this.id )
    }

    this.make_anim = function (id, frames, duration) {
        this.anims[id] = { frames : frames, duration: duration }
    }


    this.stop_anim = function () {
        this.play_anim("")
    }

    this.play_anim = function (anim_name) {
        this.frame = 0
        this.frame_timer = 0
        
        this.cur_anim = this.anims[anim_name]
        if (this.cur_anim ) {
            this.tile = this.cur_anim.frames[this.frame]
            flip(this.id, this.tile)
        }
    }

    this.on_frame = function (delta_ms) {
        if (this.cur_anim) {
            this.frame_timer += delta_ms
            if (this.frame_timer > this.cur_anim.duration) {
                this.frame_timer = 0
                this.frame += 1
                this.frame %= this.cur_anim.frames.length
                this.startTime = new Date()
                this.tile = this.cur_anim.frames[this.frame]
                flip(this.id, this.tile)
            }
        }
        this.pos = this.pos.add(this.vel)
        pos(this.id, this.pos.x, this.pos.y)

        this.vel.x *= this.speed
        this.vel.y *= this.speed

    }

}