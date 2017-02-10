@once-set-sdf = 0
	sdf 1, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@change-ter = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 40, -1
	ex2 147, -1, -1
	goto -1
@once-set-sdf = 2
	sdf 1, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 3
@change-ter = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 39, -1
	ex2 147, -1, -1
	goto -1
@once-town-encounter = 4
	sdf 1, 1
	msg 0, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 5
	sdf 1, 2
	msg 1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 6
	sdf 1, 3
	msg 2, -1, -1
	pic 27, 4
	ex1 1, 2, -1
	ex2 20, -1, -1
	goto -1
@once-trap = 7
	sdf 1, 4
	msg 2, -1, -1
	pic 27, 4
	ex1 2, 2, -1
	ex2 30, -1, -1
	goto -1
@once-trap = 8
	sdf 1, 5
	msg 2, -1, -1
	pic 27, 4
	ex1 3, 1, -1
	ex2 40, -1, -1
	goto -1
@lever-generic = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 10, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 1, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 11, -1
	ex2 -1, -1, -1
	goto 12
@disp-msg = 11
	sdf -1, -1
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@play-sound = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, -1, -1
	ex2 -1, -1, -1
	goto 13
@once-town-encounter = 13
	sdf 1, 6
	msg 4, -1, -1
	pic 0, 4
	ex1 3, -1, -1
	ex2 -1, -1, -1
	goto 14
@rect-change-ter = 14
	sdf 84, 100
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 34, -1
	ex2 16, 36, -1
	goto -1
@if-sdf = 15
	sdf 1, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 14, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 16
	sdf 1, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 17, -1
	goto -1
@block-move = 17
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto 18
@damage = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 6, -1
	ex2 0, 5, -1
	goto -1
@if-context = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 108, 21
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 20
	sdf 1, 7
	msg 6, 8, -1
	pic 0, 4
	ex1 4, -1, -1
	ex2 -1, -1, -1
	goto 23
@if-sdf = 21
	sdf 1, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 22, -1
	ex2 -1, -1, -1
	goto 20
@disp-msg = 22
	sdf -1, -1
	msg 7, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@xp = 23
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 24
	sdf 1, 8
	msg 9, -1, -1
	pic 0, 4
	ex1 4, 0, -1
	ex2 -1, -1, -1
	goto -1
@portal = 25
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 35, 15, -1
	ex2 -1, -1, -1
	goto 26
@hp = 26
	sdf -1, -1
	msg 16, -1, -1
	pic 0, 4
	ex1 50, 1, -1
	ex2 -1, -1, -1
	goto -1
@move-party = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 35, 15, -1
	ex2 1, -1, -1
	goto -1
@once-dlog = 28
	sdf -1, -1
	msg 17, -1, 1
	pic 170, 1
	ex1 23, 29, -1
	ex2 -1, -1, -1
	goto -1
@change-time = 29
	sdf -1, -1
	msg 23, 24, -1
	pic 0, 4
	ex1 3000, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 30
	sdf -1, -1
	msg 17, -1, 1
	pic 165, 1
	ex1 23, 31, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 31
	sdf -1, -1
	msg 25, 26, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 32
	sdf 1, 9
	msg 27, -1, 1
	pic 208, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 33
	sdf -1, -1
	msg 33, 34, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
