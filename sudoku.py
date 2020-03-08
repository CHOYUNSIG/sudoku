import time

a=[[0]*9 for row in range(9)]

def refresh():
	for i in range(0,9):
		for i2 in range(0,9):
			a[i][i2]=0

def errorSearch():
	for x in range(0,9):
		for y1 in range(0,8):
			for y2 in range(y1+1,9):
				if a[x][y1]!=0 and a[x][y2]!=0 and (a[x][y1]==a[x][y2] or a[x][y1]+10==a[x][y2] or a[x][y1]==a[x][y2]+10):
					return 1
	for y in range(0,9):
		for x1 in range(0,8):
			for x2 in range(x1+1,9):
				if a[x1][y]!=0 and a[x2][y]!=0 and (a[x1][y]==a[x2][y] or a[x1][y]+10==a[x2][y] or a[x1][y]==a[x2][y]+10):
					return 1
	for ax in range(0,3):
		for ay in range(0,3):
			ex=list()
			for x in range(ax*3,ax*3+3):
				for y in range(ay*3,ay*3+3):
					if a[x][y]!=0:
						if a[x][y]>10:
							ex.append(a[x][y]-10)
						else:
							ex.append(a[x][y])
			if len(set(ex))!=len(ex):
				return 1
			del ex
	return 0

def statePrint():
	print()
	print(" | 1 2 3 | 4 5 6 | 7 8 9",end="")
	for y in range(0,9):
		print()
		if y%3==0:
			print("-+-------+-------+-------")
		print(chr(65+y),end="")
		x=0
		for c in range(0,12):
			if c%4==0:
				print("|",end=" ")
			elif a[x][y]==0:
				print(" ",end=" ")
				x=x+1
			elif a[x][y]>10:
				print(a[x][y]-10,end=" ")
				x=x+1
			else:
				print(a[x][y],end=" ")
				x=x+1
	print()

def sudoku():
	p=[0,0]
	while a[p[0]][p[1]]!=0:
		if p[0]==8:
			p[0]=0
			p[1]=p[1]+1
		else:
			p[0]=p[0]+1
	#initial pivot setting
	key=0
	while key!=1:
		a[p[0]][p[1]]=a[p[0]][p[1]]+1
		if a[p[0]][p[1]]==10:
			a[p[0]][p[1]]=0
			while a[p[0]][p[1]]>10 or a[p[0]][p[1]]==0:
				if p[0]==0:
					p[0]=8
					p[1]=p[1]-1
				else:
					p[0]=p[0]-1
		elif errorSearch()==0:
			while a[p[0]][p[1]]!=0:
				if p[0]==8:
					if p[1]==8:
						key=1
						break
					else:
						p[0]=0
						p[1]=p[1]+1
				else:
					p[0]=p[0]+1

#1. add 1 to pivot
#2. if it got 10, erase it and go back
#3. if not, move pivot to next

def op():
	while True:
		statePrint()
		print()
		xy=input("$ Location : ")
		if xy=='0':
			print('$ processing..')
			st=time.time()
			sudoku()
			t=(time.time()-st)*1000
			print()
			print("$ Solution")
			statePrint()
			print()
			print("$ time : {}ms".format(t))
			print()
			print('-'*30)
			break
		elif len(xy)==2 and 64<ord(xy[0])<74 and 48<ord(xy[1])<58:
			v=input("$ Value : ")
			if 48<ord(v)<58:
				a[ord(xy[1])-49][ord(xy[0])-65]=int(v)+10
				if errorSearch()==1:
					a[ord(xy[1])-49][ord(xy[0])-65]=0
					print("$ error")
			else:
				print("$ error")
		else:
			print("$ error")

#main loop
print('SUDOKU ALGORITHM in python 3')
print('$ enter preset to use')
print('$ if you want to get a solution,')
print('$ put 0 at location value')
while True:
	refresh()
	op()
