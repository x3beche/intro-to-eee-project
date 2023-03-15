# Artık detect.py yok bu kodu çalıştıracaz
import torch, cv2, os, time
from collections import deque
import serial

ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)

model = torch.hub.load(
    os.getcwd(), "custom", source="local", path="best.pt", force_reload=True
)

labels = model.names
a = {0: "stopsign_1", 1: "speed5sign_5", 2: "speed10sign_11", 3: "speed20sign_20"}
predictions = deque([], maxlen=10)

cap = cv2.VideoCapture("http://192.168.43.225:81/stream")


time.sleep(2)
while True:
    ret, photo = cap.read()

    results = model(photo)

    for pred in results.pred[0]:
        if pred[4] > 0.8:
            cv2.rectangle(
                photo,
                (int(pred[0].item()), int(pred[1].item())),
                (int(pred[2].item()), int(pred[3].item())),
                (0, 255, 0),
                3,
            )
            predictions.append(int(pred[5]))
            final_prediction = round(sum(predictions) / len(predictions))
            cv2.putText(
                photo,
                labels[final_prediction],
                (50, 50),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (255, 0, 0),
                2,
                cv2.LINE_AA,
            )
            ser.write(
                int(a[final_prediction].split("_")[1]).to_bytes(1, "little")
                + "\n".encode()
            )
        # print(pred[0].item())

    cv2.imshow("streaming", photo)

    if cv2.waitKey(1) == ord("q"):
        break

"""class Sign:
    which = str
    area = float

    def __init__(self, which = "",  area=0):
        self.which = which
        self.area = area


sign1 = Sign()
sign2 = Sign()
sign3 = Sign    if cv2.waitKey(1) == ord("q"):
        break()
sign4 = Sign()
sign5 = Sign()

stream = cv2.VideoCapture(0)
time.sleep(0.1)
signs_dict = {0: "D", 1:"stop", 2: "left"}
signs_list = []

while True:
    ret, frame = stream.read()
    cv2.imshow("frame", frame)
    results = model(frame)

    if len(results.pred[0]) > 0:
        if results.pred[0][0][4].item() > 0.6:
            x1 = results.pred[0][0][0].item()
            y1 = results.pred[0][0][1].item()
            x2 = results.pred[0][0][2].item()
            y2 = results.pred[0][0][3].item()
            area = abs(x1 - x2) * abs(y1 - y2)
            print("area: ", area, signs_dict[int(results.pred[0][0][5])])
            signs_list.append(area)
            
    if len(signs_list) == 7:
                    print("*************", max(signs_list))
                    signs_list.clear()
                            
    if cv2.waitKey(1) == ord("q"):
        break"""
