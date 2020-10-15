// anim obj {frames:[], duration:-1}

Sprite = function (tile ) {

    //move these to physics proto
    this.pos = new vec2(0, 0)

    this.frame_timer = 0
    this.frame = 0 //index in animation
    this.tile = tile
    this.anims = {} 
    this.cur_anim = null
    this.id = SprSpawn(tile, 0, 0)
    this.size = new vec2( SprSize(this.id) )

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
        this.pos.x += dx
        this.pos.y += dy
    }

    this.move_to = function (x, y) {
        this.pos = new vec2(x, y)
    }

    this.kill = function () {
        SprKill( this.id )
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
            SprFlip(this.id, this.tile)
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
                SprFlip(this.id, this.tile)
            }
        }

        SprPos(this.id, this.pos.x, this.pos.y)
    }

}