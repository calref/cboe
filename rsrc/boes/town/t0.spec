
# Sample special node sequence.

def Varn = 3

@if-gold            # The first node is node 0
    ex1  50, 5      # If they have 50 gold, take it and jump to node 5
    goto 1          # Otherwise, jump to node 1
@disp-msg           # Subsequent nodes increment from the previous one
    msg  2          # Shows string 2; the omitted argument is assumed to be -1
@town-visible = 5   # Explicitly specify node number
    msg  3,4        # Shows strings 3 and 4 in the message dialog
    ex1  Varn,1     # Town 3, set visibility to true