void print();

int i,j;

int main(void)
{
    i = 1;
    j = 2;
    print(i+j); // 3
    print(j-i); // 1
    print(i*j); // 2
    print(i*j-i); // 1
    print( j-i*j); // 0
}
