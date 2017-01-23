@if-ter = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 42, -1
	ex2 145, 1, -1
	goto -1
@if-spec-item = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 2, -1
	ex2 -1, -1, -1
	goto 3
@change-ter = 2
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 13, 42, -1
	ex2 142, -1, -1
	goto -1
@damage = 3
	sdf -1, -1
	msg 1, 2, -1
	pic 0, 4
	ex1 7, 6, -1
	ex2 8, 1, -1
	goto -1
@once-trap = 4
	sdf 20, 1
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 60, -1, -1
	goto -1
@once-trap = 5
	sdf 20, 2
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 70, -1, -1
	goto -1
@once-dlog = 6
	sdf -1, -1
	msg 3, -1, 1
	pic 135, 1
	ex1 19, 7, -1
	ex2 -1, -1, -1
	goto -1
@damage = 7
	sdf -1, -1
	msg 9, 10, -1
	pic 0, 4
	ex1 6, 7, -1
	ex2 6, 1, -1
	goto -1
@once-dlog = 8
	sdf -1, -1
	msg 11, -1, 1
	pic 130, 1
	ex1 34, 9, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 17, -1, 1
	pic 130, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@lever-generic = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 11, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 25, -1
	ex2 147, 148, -1
	goto 12
@swap-ter = 12
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 40, 26, -1
	ex2 147, 148, -1
	goto -1
@lever-generic = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 14, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 45, 25, -1
	ex2 147, 148, -1
	goto 15
@swap-ter = 15
	sdf -1, -1
	msg 24, -1, -1
	pic 0, 4
	ex1 45, 26, -1
	ex2 147, 148, -1
	goto -1
@once-disp-msg = 16
	sdf 20, 4
	msg 25, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 17
	sdf -1, -1
	msg 26, -1, 1
	pic 1004, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
@set-sdf = 18
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 19
@once-give-spec-item = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 0, -1
	ex2 -1, -1, -1
	goto 20
@xp = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 21
	sdf 20, 5
	msg 32, -1, 1
	pic 1004, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 22
@once-town-encounter = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 23
	sdf -1, -1
	msg 38, -1, 1
	pic 244, 1
	ex1 24, 24, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 24
	sdf 20, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 25, -1
	ex2 -1, -1, -1
	goto 27
@disp-msg = 25
	sdf -1, -1
	msg 44, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 26
@move-party = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 60, -1
	ex2 0, -1, -1
	goto -1
@set-sdf = 27
	sdf 20, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 28
@disp-msg = 28
	sdf -1, -1
	msg 45, 46, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 26
@if-sdf = 29
	sdf 20, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 32, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 30
	sdf 20, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 31, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 31
	sdf 20, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 32
	sdf -1, -1
	msg 47, 48, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 33
@death = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@make-wandering = 34
	sdf -1, -1
	msg 49, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 35
	sdf 20, 8
	msg 50, -1, 1
	pic 105, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 36
	sdf 20, 9
	msg 56, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 37
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 38, -1
	ex2 -1, -1, -1
	goto 39
@disp-msg = 38
	sdf -1, -1
	msg 57, 58, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 39
	sdf -1, -1
	msg 59, 60, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 40
@death = 40
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 41
	sdf -1, -1
	msg 61, -1, 1
	pic 196, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 42
	sdf 20, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 34, -1
	ex2 -1, -1, -1
	goto -1
