@if-sdf = 0
	sdf 101, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 2
@death = 1
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 6, 0
	msg 2, 3, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 4
@once-dlog = 4
	sdf -1, -1
	msg 4, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 5
	sdf 6, 1
	msg 10, 11, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 6
	sdf 6, 2
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 7
	sdf 6, 3
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 8
	sdf 6, 4
	msg 14, 15, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 9
	sdf -1, -1
	msg 16, 17, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 10
@change-ter = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 44, 16, -1
	ex2 128, -1, -1
	goto -1
@disp-msg = 11
	sdf -1, -1
	msg 18, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 12
	sdf -1, -1
	msg 19, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf 6, 5
	msg 21, -1, 1
	pic 133, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 17, -1
	ex2 -1, -1, -1
	goto 16
@disp-msg = 16
	sdf -1, -1
	msg 27, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 17
	sdf -1, -1
	msg 28, -1, 1
	pic 107, 1
	ex1 45, 18, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 18
	sdf -1, -1
	msg 34, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 19
@anim-explode = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 10, -1
	ex2 0, -1, -1
	goto 20
@anim-explode = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 32, 7, -1
	ex2 0, -1, -1
	goto 21
@anim-explode = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 32, 11, -1
	ex2 0, -1, -1
	goto 22
@once-dlog = 22
	sdf -1, -1
	msg 40, -1, 1
	pic 17, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 23
@damage = 23
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 8, -1
	ex2 4, 0, -1
	goto 24
@once-dlog = 24
	sdf -1, -1
	msg 46, -1, 1
	pic 107, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 34
@once-disp-msg = 25
	sdf 6, 6
	msg 52, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 26
	sdf 0, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 27, -1
	ex2 -1, -1, -1
	goto 28
@disp-msg = 27
	sdf -1, -1
	msg 53, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 29, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 29
	sdf -1, -1
	msg 54, 55, -1
	pic 0, 4
	ex1 12, 1, -1
	ex2 -1, -1, -1
	goto 30
@set-sdf = 30
	sdf 0, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 31
@gold = 31
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto 32
@xp = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 0, -1
	ex2 -1, -1, -1
	goto -1
@stair = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 23, -1
	ex2 9, 1, -1
	goto -1
@once-give-spec-item = 34
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 1, -1
	ex2 -1, -1, -1
	goto 35
@set-sdf = 35
	sdf 101, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 33
