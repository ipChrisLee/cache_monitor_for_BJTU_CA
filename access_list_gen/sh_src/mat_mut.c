#define N 32
int a[N][N], b[N][N], c[N][N];
int main(){
	for(int i=0;i<N;++i){
		for(int j=0;j<N;++j){
			for(int k=0;k<N;++k){
				a[i][j]+=b[i][k]*c[k][j];
			}
		}
	}
	return 0;
}
