#ifndef FF_TIMESTAMP_H_
#define FF_TIMESTAMP_H_

#include <stdint.h>
#include <string>

namespace firey{

class ffTimeStamp{
	private:
		int64_t usSinceEpoch_;

	public:
		ffTimeStamp()
			:usSinceEpoch_(0)
		{
		}

		explicit ffTimeStamp(int64_t arg)
			:usSinceEpoch_(arg)
		{
		}

		void swap(ffTimeStamp& other){
			std::swap(usSinceEpoch_,other.usSinceEpoch_);
		}

		std::string toString() const;
		std::string toFormatString(bool show_us=true) const;

		static ffTimeStamp now();
		static ffTimeStamp invalid(){return ffTimeStamp();}

		static const int kusPerSecond = 1000*1000;


		static ffTimeStamp fromUnixTime(time_t t){
			return fromUnixTime(t,0);
		}
		
		//second to microsecond
		static ffTimeStamp fromUnixTime(time_t t,int us){
			return ffTimeStamp(static_cast<int>(t)*kusPerSecond+us);
		}

		int64_t usSinceEpoch(){return usSinceEpoch_;}
};

inline bool operator<(ffTimeStamp lhs,ffTimeStamp rhs){
	return lhs.usSinceEpoch()<rhs.usSinceEpoch();
}

inline bool operator==(ffTimeStamp lhs,ffTimeStamp rhs){
	return lhs.usSinceEpoch()==rhs.usSinceEpoch();
}

inline bool operator>(ffTimeStamp lhs,ffTimeStamp rhs){
	return lhs.usSinceEpoch()>rhs.usSinceEpoch();
}

//获取两个时间的差值（秒为单位）
inline double timeDifference(ffTimeStamp high,ffTimeStamp low){
	int64_t diff=high.usSinceEpoch()-low.usSinceEpoch();
	return static_cast<double>(diff)/ffTimeStamp::kusPerSecond;
}

inline ffTimeStamp addTime(ffTimeStamp time,double sec){
	int64_t second=static_cast<int64_t>(sec*ffTimeStamp::kusPerSecond);
	return ffTimeStamp(time.usSinceEpoch()+second);
}

}//namespace firey

#endif //FF_TIMESTAMP_H_
