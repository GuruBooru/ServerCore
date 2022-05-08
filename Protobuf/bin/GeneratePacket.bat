pushd %~dp0

protoc.exe --proto_path=./ --cpp_out=./ ./Enum.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto

PacketGenerator.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_
PacketGenerator.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../TestServer"
XCOPY /Y Enum.pb.cc "../../TestServer"
XCOPY /Y Struct.pb.h "../../TestServer"
XCOPY /Y Struct.pb.cc "../../TestServer"
XCOPY /Y Protocol.pb.h "../../TestServer"
XCOPY /Y Protocol.pb.cc "../../TestServer"
XCOPY /Y ClientPacketHandler.h "../../TestServer"

XCOPY /Y Enum.pb.h "../../DummyClient"
XCOPY /Y Enum.pb.cc "../../DummyClient"
XCOPY /Y Struct.pb.h "../../DummyClient"
XCOPY /Y Struct.pb.cc "../../DummyClient"
XCOPY /Y Protocol.pb.h "../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../DummyClient"
XCOPY /Y ServerPacketHandler.h "../../DummyClient"

DEL /Q /F *.pb.h
DEL /Q /F *.ph.cc
DEL /Q /F *.h

PAUSE