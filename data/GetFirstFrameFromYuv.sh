# /bin/zsh

if [ ! -d "RawImages" ]; then
    mkdir RawImages
    chmod 777 RawImages
else
    echo "Folder already exists"
fi

# 获取每个 yuv 视频的第一帧
ffmpeg -s 3840x2160 -pix_fmt yuv420p -i Boxer-IrishMan-Gladiator.yuv -frames:v 1 ./RawImages/Boxer-IrishMan-Gladiator.png
ffmpeg -s 3840x2160 -pix_fmt yuv420p -i chess.yuv -frames:v 1 ./RawImages/chess.png
ffmpeg -s 3840x2160 -pix_fmt yuv420p -i ChessPieces.yuv -frames:v 1 ./RawImages/ChessPieces.png
ffmpeg -s 2048x2048 -pix_fmt yuv420p -i NagoyaDataLeading.yuv -frames:v 1 ./RawImages/NagoyaDataLeading.png
ffmpeg -s 2048x2048 -pix_fmt yuv420p -i NagoyaFujita.yuv -frames:v 1 ./RawImages/NagoyaFujita.png
ffmpeg -s 2048x2048 -pix_fmt yuv420p -i NagoyaOrigami.yuv -frames:v 1 ./RawImages/NagoyaOrigami.png
ffmpeg -s 2048x2048 -pix_fmt yuv420p -i Tunnel_Train.yuv -frames:v 1 ./RawImages/Tunnel_Train.png
