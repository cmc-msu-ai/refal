#pragma once

namespace Refal2 {

class CErrors {
public:
	CErrors(): errors( false ) {}

	inline void SetErrors();
	void ResetErrors() { errors = false; }
	bool HasErrors() const { return errors; }
	
protected:
	virtual void OnErrors() {};
	
private:
	CErrors(const CErrors&);
	CErrors& operator=(const CErrors&);

	bool errors;
};

inline void CErrors::SetErrors()
{
	if( !errors ) {
		errors = true;
		OnErrors();
	}
}

} // end of namespace Refal2
