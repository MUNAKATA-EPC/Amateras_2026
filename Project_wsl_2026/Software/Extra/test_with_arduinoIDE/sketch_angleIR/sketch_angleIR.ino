void setup() {
  Serial.begin(9600);  //  シリアルモニターを使用いたす
}

void loop() {

  int ch=0;  //  変数を宣言

  for( int i = 1; i <= 7; i++ ){

    if( analogRead(i) < analogRead(ch) )ch = i;

  }//最小の値を有する IRセンサー を検知するプログラム

  Serial.print("minIR is ch");  //  シリアルモニターに表示1
  Serial.println(ch);  //  シリアルモニターに表示2

}
