echo Hola $USER
echo Tu directorio de trabajo actual es $PWD
cd /home
echo Nos movimos a $PWD
cd -
echo Volvimos a $PWD
echo Mostramos los archivos en el directorio
ls -lh
echo Lanzamos unos sleep en segundo plano
sleep 10 &
sleep 8 &
sleep 6 &
sleep 4 &
sleep 2 &
echo Mostramos los trabajos activos
jobs
echo Esperamos a que finalicen todos y terminamos el programa