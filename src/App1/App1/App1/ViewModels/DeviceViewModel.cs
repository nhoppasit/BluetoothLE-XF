using Plugin.BLE.Abstractions.Contracts;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using App1.Models;

namespace App1.ViewModels
{
    public class DeviceViewModel : INotifyPropertyChanged
    {
        private IDevice _nativeDevice;


        public event PropertyChangedEventHandler PropertyChanged;
        

        public IDevice NativeDevice
        {
            get
            {
                return _nativeDevice;
            }
            set
            {
                _nativeDevice = value;
                RaisePropertyChanged();
            }
        }
        protected void RaisePropertyChanged([CallerMemberName] string caller = "")
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(caller));
            }
        }

    }
}
