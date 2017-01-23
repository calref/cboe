@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 178, 1
	ex1 49, 1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 1
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 2
	sdf -1, -1
	msg 7, -1, 1
	pic 178, 1
	ex1 49, 3, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 3
	sdf -1, -1
	msg 13, 14, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 7, -1, 1
	pic 178, 1
	ex1 49, 5, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 5
	sdf 16, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 6
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 7
	sdf 16, 0
	msg 16, 17, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf 16, 1
	msg 18, -1, 1
	pic 179, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 9
	sdf 16, 3
	msg 24, -1, -1
	pic 0, 4
	ex1 15, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 10
	sdf 16, 2
	msg 25, 26, -1
	pic 0, 4
	ex1 14, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 11
	sdf 16, 4
	msg 27, -1, 1
	pic 117, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@lever-generic = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 13, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 13
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 16, 13, -1
	ex2 130, 131, -1
	goto -1
@lever-generic = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 15, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 15
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 22, 13, -1
	ex2 130, 131, -1
	goto -1
@lever-generic = 16
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 17, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 17
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 28, 13, -1
	ex2 130, 131, -1
	goto -1
@lever-generic = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 19, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 19
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 34, 13, -1
	ex2 130, 131, -1
	goto -1
@if-ter = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 1, -1
	ex2 128, 21, -1
	goto -1
@if-spec-item = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 25, -1
	ex2 -1, -1, -1
	goto 22
@disp-msg = 22
	sdf -1, -1
	msg 34, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 23
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 1, -1
	ex2 128, 58, -1
	goto -1
@change-ter = 24
	sdf -1, -1
	msg 35, -1, -1
	pic 0, 4
	ex1 25, 1, -1
	ex2 125, -1, -1
	goto -1
@change-ter = 25
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 18, 1, -1
	ex2 125, -1, -1
	goto -1
@once-dlog = 26
	sdf -1, -1
	msg 37, -1, 1
	pic 135, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 28, -1
	ex2 11, 0, -1
	goto 29
@spell-mage = 28
	sdf -1, -1
	msg 43, 44, -1
	pic 0, 4
	ex1 61, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 29
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 30
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 4, -1
	ex2 130, 31, -1
	goto -1
@disp-msg = 31
	sdf -1, -1
	msg 46, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 4, -1
	ex2 130, 33, -1
	goto -1
@if-spec-item = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 35, -1
	ex2 -1, -1, -1
	goto 34
@disp-msg = 34
	sdf -1, -1
	msg 47, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 35
	sdf -1, -1
	msg 48, -1, 1
	pic 92, 1
	ex1 45, 36, -1
	ex2 -1, -1, -1
	goto -1
@damage = 36
	sdf -1, -1
	msg 54, -1, -1
	pic 0, 4
	ex1 6, 6, -1
	ex2 6, 1, -1
	goto 37
@anim-explode = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 4, -1
	ex2 0, -1, -1
	goto 38
@change-ter = 38
	sdf -1, -1
	msg 55, -1, -1
	pic 0, 4
	ex1 6, 4, -1
	ex2 131, -1, -1
	goto 50
@if-ter = 39
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 44, -1
	ex2 131, 40, -1
	goto -1
@change-ter = 40
	sdf -1, -1
	msg 56, 57, -1
	pic 0, 4
	ex1 41, 44, -1
	ex2 130, -1, -1
	goto -1
@if-ter = 41
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 44, -1
	ex2 130, 42, -1
	goto -1
@if-spec-item = 42
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 44, -1
	ex2 -1, -1, -1
	goto 43
@disp-msg = 43
	sdf -1, -1
	msg 58, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 44
	sdf 16, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 45, -1
	ex2 -1, -1, -1
	goto 46
@disp-msg = 45
	sdf -1, -1
	msg 59, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 46
	sdf -1, -1
	msg 60, -1, 1
	pic 92, 1
	ex1 45, 47, -1
	ex2 -1, -1, -1
	goto -1
@damage = 47
	sdf -1, -1
	msg 66, -1, -1
	pic 0, 4
	ex1 6, 6, -1
	ex2 6, 1, -1
	goto 48
@anim-explode = 48
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 44, -1
	ex2 0, -1, -1
	goto 49
@once-give-spec-item = 49
	sdf 16, 5
	msg 67, -1, -1
	pic 0, 4
	ex1 16, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 50
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 51
	sdf 16, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 53, -1
	ex2 -1, -1, -1
	goto 52
@once-give-dlog = 52
	sdf 16, 7
	msg 74, -1, 16
	pic 125, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 53
	sdf -1, -1
	msg 68, -1, 1
	pic 125, 1
	ex1 19, 54, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 54
	sdf 16, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 0, -1
	ex2 -1, -1, -1
	goto -1
@once = 55
	sdf 16, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 56
@if-spec-item = 56
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 57, -1
	ex2 -1, -1, -1
	goto 51
@disp-msg = 57
	sdf -1, -1
	msg 80, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 58
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 14, 24, -1
	ex2 -1, -1, -1
	goto 22
