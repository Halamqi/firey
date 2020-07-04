#ifndef FF_TIMESTAMP_H_
#define FF_TIMESTAMP_H_

#include <stdint.h>
#include <string>

namespace firey{

class Timestampff{
	private:
		int64_t usSinceEpoch_;

	public:
		Timestampff()
			:usSinceEpoch_(0)
		{
		}

		explicit Timestampff(int64_t arg)
			:usSinceEpoch_(arg)
		{
		}

		void swap(Timestampff& other){
			std::swap(usSinceEpoch_,other.usSinceEpoch_);
		}

		std::string toString() const;
		std::string toFormatString(bool show_us=true) const;

		static Timestampff now();
		static Timestampff invalid(){return Timestampff();}

		static const int kusPerSecond = 1000*1000;


		/*static Timestampff fromUnixTime(time_t t){
			return fromUnixTime(t,0);
		}
		
		//second to microsecond
		static Timestampff fromUnixTime(time_t t,int us){
			return Timestampff(static_cast<int>(t)*kusPerSecond+us);
		}*/

		int64_t usSinceEpoch(){return usSinceEpoch_;}
};

inline bool operator<(Timestampff lhs,Timestampff rhs){
	return lhs.usSinceEpoch()<rhs.usSinceEpoch();
}

inline bool operator==(Timestampff lhs,Timestampff rhs){
	return lhs.usSinceEpoch()==rhs.usSinceEpoch();
}

inline bool operator>(Timestampff lhs,Timestampff rhs){
	return lhs.usSinceEpoch()>rhs.usSinceEpoch();
}

//获取两个时间的差值（秒为单位）
inline double timeDifference(Timestampff high,Timestampff low){
	int64_t diff=high.usSinceEpoch()-low.usSinceEpoch();
	return static_cast<double>(diff)/Timestampff::kusPerSecond;
}

inline Timestampff addTime(Timestampff time,double sec){
	int64_t second=static_cast<int64_t>(sec*Timestampff::kusPerSecond);
	return Timestampff(time.usSinceEpoch()+second);
}

}//namespace firey

#endif //FF_TIMESTAMP_H_
